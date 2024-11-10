#pragma once

namespace MqttConstants{
constexpr int HEAD_LENGHT = 2;
};

namespace MqttProtocol{

enum MqttMessageType{
    CONNECT = 1,
    CONNACK = 2,
    PUBLISH = 3,
    PUBACK = 4,
    PUBREC = 5,
    PUBREL = 6,
    PUBCOMP = 7,
    SUBSCRIBE = 8,
    SUBACK = 9,
    UNSUBSCRIBE = 10,
    UNSUBACK = 11,
    PINGREQ = 12,
    PINGRESP = 13,
    DISCONNECT = 14,
    AUTH = 15
};

class MqttMessage{
};

template<typename bytes_stream>
class IMqttMessageParser{
public:
    using ptr_message = std::shared_ptr<MqttMessage>;
    virtual ptr_message parse_message(bytes_stream&) = 0;
};

template<typename bytes_stream>
class MqttPublishParser : public IMqttMessageParser<bytes_stream>{
public:
    typename IMqttMessageParser<bytes_stream>::ptr_message parse_message(bytes_stream& stream) override{
        return {};
    }
};

template<typename bytes_stream>
class MqttSubscribeParser : public IMqttMessageParser<bytes_stream>{
    typename IMqttMessageParser<bytes_stream>::ptr_message parse_message(bytes_stream& stream) override{
        return {};
    }
};

template<typename bytes_stream>
class MqttConnectionParser : public IMqttMessageParser<bytes_stream>{
    typename IMqttMessageParser<bytes_stream>::ptr_message parse_message(bytes_stream& stream) override{
        return {};
    }
};

template<typename bytes_stream>
class MqttMessageParserFabric{
public:
    using ptr = std::shared_ptr<IMqttMessageParser<bytes_stream>>;

    ptr get_parser(MqttMessageType msg_type){
        switch (msg_type)
        {
        case MqttMessageType::PUBLISH:
            return std::make_shared<MqttPublishParser<bytes_stream>>(); break;
        case MqttMessageType::SUBSCRIBE:
            return std::make_shared<MqttPublishParser<bytes_stream>>(); break;
        case MqttMessageType::CONNECT:
            return std::make_shared<MqttConnectionParser<bytes_stream>>(); break;
        default:
            return std::make_shared<MqttPublishParser<bytes_stream>>(); break;
        }

        return {};
    }
};

template<typename bytes_stream>
class MqttParser{
public:
    bool try_parse_message_from_bytes(bytes_stream& stream){
        char head[MqttConstants::HEAD_LENGHT];
        if(!stream.read(head, MqttConstants::HEAD_LENGHT)){
            return false;
        }
        int payload_size = head[1];
        char payload[payload_size];

        if(!stream.read(payload_size, payload)){
            return false;
        }

        auto type = read_message_type_from_byte(head[0]);

        MqttMessageParserFabric<bytes_stream> parser_fabric;

        auto parser = parser_fabric.get_parser(type);
        auto mqtt_message = parser->parse_message(stream);

        return true;
    }

private:
    MqttMessageType read_message_type_from_byte(char byte){
        byte = (4 >> byte); 
        return static_cast<MqttMessageType>(byte);
    }
};

}
