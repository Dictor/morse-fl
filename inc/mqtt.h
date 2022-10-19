#ifndef HANGANG_VIEW_MQTT
#define HANGANG_VIEW_MQTT

#include <zephyr/net/mqtt.h>
#include <zephyr/net/net_ip.h>

#include <map>

namespace hangang_view {
class MQTTClient {
 private:
  static const int buffer_size_ = 256;
  uint8_t rx_buffer_[buffer_size_];
  uint8_t tx_buffer_[buffer_size_];
  uint8_t payload_[buffer_size_];
  struct mqtt_client client_;
  struct sockaddr_storage broker_;

  atomic_t mqtt_status_;
  enum MQTTStatusBit {
    kBitConnected = 0,
    kBitError = 1,
  };

 public:
  static std::map<struct mqtt_client *, MQTTClient *> callback_table_;
  MQTTClient(in_addr address, uint16_t port) {
    mqtt_status_ = ATOMIC_INIT(0);

    /* Init address struct */
    struct sockaddr_in *broker4 = (struct sockaddr_in *)&broker_;
    broker4->sin_family = AF_INET;
    broker4->sin_port = htons(port);
    broker4->sin_addr = address;

    /* MQTT client configuration */
    mqtt_client_init(&client_);
    client_.broker = &broker_;
    client_.evt_cb = &hangang_view::MQTTClient::GlobalEventCallback;
    client_.client_id.utf8 = (uint8_t *)"zephyr_mqtt_client";
    client_.client_id.size = sizeof("zephyr_mqtt_client") - 1;
    client_.password = NULL;
    client_.user_name = NULL;
    client_.protocol_version = MQTT_VERSION_3_1_1;
    client_.transport.type = MQTT_TRANSPORT_NON_SECURE;

    MQTTClient::callback_table_.insert(
        std::pair<struct mqtt_client *, MQTTClient *>(&client_, this));

    /* MQTT buffers configuration */
    client_.rx_buf = rx_buffer_;
    client_.rx_buf_size = sizeof(rx_buffer_);
    client_.tx_buf = tx_buffer_;
    client_.tx_buf_size = sizeof(tx_buffer_);
  };

  int Connect();
  int WaitEstablished(int timeout);
  int Subscribe(char* topic_name);
  int Abort();

  static void GlobalEventCallback(struct mqtt_client *client,
                                  const struct mqtt_evt *evt);
  void EventCallback(const struct mqtt_evt *evt);
  bool IsConnected();
  bool HasError();
};
};  // namespace hangang_view
#endif
