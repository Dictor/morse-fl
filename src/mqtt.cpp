#include "../inc/mqtt.h"

#include <zephyr/logging/log.h>
#include <zephyr/net/socket.h>
#include <zephyr/random/rand32.h>

#include "../inc/json.h"

LOG_MODULE_REGISTER(mqtt);

using namespace hangang_view;

std::map<struct mqtt_client *, MQTTClient *> MQTTClient::callback_table_;

void MQTTClient::GlobalEventCallback(struct mqtt_client *client,
                                     const struct mqtt_evt *evt) {
  auto v = MQTTClient::callback_table_.find(client);
  if (v == MQTTClient::callback_table_.end()) {
    LOG_ERR("given client doesn't exist on callback table : 0x%x", client);
    return;
  }
  LOG_DBG("mqtt callback fired, client : 0x%x", client);
  v->second->EventCallback(evt);
}

void MQTTClient::EventCallback(const struct mqtt_evt *evt) {
  switch (evt->type) {
    case MQTT_EVT_CONNACK:
      if (evt->result != 0) {
        LOG_ERR("MQTT connect failed %d", evt->result);
        atomic_set_bit(&mqtt_status_, kBitError);
        break;
      }
      atomic_set_bit(&mqtt_status_, kBitConnected);
      LOG_INF("MQTT client connected!");
      break;

    case MQTT_EVT_DISCONNECT:
      LOG_INF("MQTT client disconnected %d", evt->result);
      atomic_clear_bit(&mqtt_status_, kBitConnected);
      // clear_fds();
      break;

    case MQTT_EVT_PUBACK:
      if (evt->result != 0) {
        LOG_ERR("MQTT PUBACK error %d", evt->result);
        atomic_set_bit(&mqtt_status_, kBitError);
        break;
      }
      LOG_DBG("PUBACK packet id: %u", evt->param.puback.message_id);
      break;

    case MQTT_EVT_PUBREC: {
      if (evt->result != 0) {
        LOG_ERR("MQTT PUBREC error %d", evt->result);
        atomic_set_bit(&mqtt_status_, kBitError);
        break;
      }
      LOG_DBG("PUBREC packet id: %u", evt->param.pubrec.message_id);
      const struct mqtt_pubrel_param rel_param = {
          .message_id = evt->param.pubrec.message_id};
      if (int err = mqtt_publish_qos2_release(&client_, &rel_param) != 0) {
        LOG_ERR("Failed to send MQTT PUBREL: %d", err);
      }
      break;
    }
    case MQTT_EVT_PUBCOMP:
      if (evt->result != 0) {
        LOG_ERR("MQTT PUBCOMP error %d", evt->result);
        atomic_set_bit(&mqtt_status_, kBitError);
        break;
      }
      LOG_DBG("PUBCOMP packet id: %u", evt->param.pubcomp.message_id);
      break;

    case MQTT_EVT_PINGRESP:
      LOG_DBG("PINGRESP packet");
      break;

    case MQTT_EVT_PUBLISH: {
      int message_len = evt->param.publish.message.payload.len;

      LOG_INF("MQTT publish received %d, %d bytes", evt->result, message_len);
      LOG_INF("id: %d, qos: %d", evt->param.publish.message_id,
              evt->param.publish.message.topic.qos);

      int read_len = 0;
      if (message_len > sizeof(payload_) - 1) {  // -1 for final null character
        LOG_ERR("message payload more larger then buffer!");
        return;
      }


      while (message_len > read_len) {
        int bytes_read = mqtt_read_publish_payload(
            &client_, payload_ + read_len, message_len - read_len);
        if (bytes_read < 0 && bytes_read != -EAGAIN) {
          LOG_ERR("failure to read payload : %d", bytes_read);
          break;
        }
        read_len += bytes_read;
      }

      payload_[message_len] = '\0';
      LOG_INF("complete to read %d bytes from %d bytes of payload", read_len, message_len);
      LOG_DBG("payload : %s", payload_);

      struct mqtt_puback_param puback;
      puback.message_id = evt->param.publish.message_id;
      mqtt_publish_qos1_ack(&client_, &puback);

      int ret = json::ParseSymbolJson((char *)payload_, message_len, symbols_);
      if (ret < 0) {
        LOG_ERR("fail to parse payload to json : %d", ret);
      }
      LOG_INF("%d symbols parsed to 0x%x.", symbols_->symbols_len, symbols_);
      latest_publish_time_ = k_uptime_get();
      break;
    }
    default:
      break;
  }
}

bool MQTTClient::IsConnected() {
  return atomic_test_bit(&mqtt_status_, kBitConnected);
}

bool MQTTClient::HasError() {
  return atomic_test_bit(&mqtt_status_, kBitError);
}

int MQTTClient::Connect() { return mqtt_connect(&client_); }

int MQTTClient::WaitEstablished(int timeout) {
  struct zsock_pollfd fds[1];
  fds[0].fd = client_.transport.tcp.sock;
  fds[0].events = ZSOCK_POLLIN;
  if (int ret = zsock_poll(fds, 1, timeout) < 0) {
    LOG_ERR("poll fail: %d", ret);
    return ret;
  }
  mqtt_input(&client_);
  return 0;
}

// MQTT_UTF8_LITERAL
int MQTTClient::Subscribe(char *topic_name) {
  struct mqtt_topic topic;
  topic.topic.utf8 = (uint8_t *)topic_name;
  topic.topic.size = strlen(topic_name);
  topic.qos = MQTT_QOS_0_AT_MOST_ONCE;
  struct mqtt_subscription_list topic_list;
  topic_list.list = &topic;
  topic_list.list_count = 1U;
  topic_list.message_id = sys_rand32_get();
  return mqtt_subscribe(&client_, &topic_list);
}

int MQTTClient::Input() { return mqtt_input(&client_); }

int MQTTClient::Abort() { return mqtt_abort(&client_); }

int64_t MQTTClient::LatestPublishTime() { return latest_publish_time_; }
