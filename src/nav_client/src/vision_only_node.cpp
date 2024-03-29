////
//// Created by shuxy on 24-3-9.
////
//#include "nav_client/vision_only_node.hpp"
//
////ros2
//#include "tf2/LinearMath/Quaternion.h"
//#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
//#include "rclcpp/logging.hpp"
//#include "rclcpp/utilities.hpp"
//#include "rclcpp/qos.hpp"
//
//// std;
//#include "cstdint"
//#include "functional"
//#include "map"
//#include "memory"
//#include "string"
//#include "vector"
//
//namespace nav_client {
//    VisionClient::VisionClient(const rclcpp::NodeOptions &options) : Node("vision_client_node", options) {
//
//
//        //set socket
//        try {
//            client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
//            if (client_socket_fd < 0)
//                RCLCPP_ERROR(this->get_logger(), "Error create socket fd");
//            else
//                RCLCPP_INFO(this->get_logger(), "create socket fd");
//
//            serv_port_ = declare_parameter("serv_port", 51718);
//            serv_ip_ = declare_parameter("serv_ip", "192.168.123.147");
////
//            serv_addr_.sin_family = AF_INET;
//            serv_addr_.sin_addr.s_addr = inet_addr(serv_ip_.c_str());
//            serv_addr_.sin_port = htons(serv_port_);
//
//
////            client_addr_.sin_family = AF_INET;
////            client_addr_.sin_addr.s_addr = inet_addr(serv_ip_.c_str());
////            client_addr_.sin_addr.s_addr = INADDR_ANY;
////            client_addr_.sin_port = htons(serv_port_);
////            bind(client_socket_fd, (struct sockaddr *)&serv_addr_, sizeof (serv_addr_));
////            RCLCPP_INFO(get_logger(), "bind");
////
//
//
//
//
//            RCLCPP_INFO(get_logger(), "send to %s:%d", serv_ip_.c_str(), serv_port_);
////            if(connect(client_socket_fd, (struct sockaddr *)&serv_addr_, sizeof(serv_addr_)) < 0)
////                RCLCPP_ERROR(get_logger(), "error connect");
//
//        } catch (const std::exception &ex) {
//            RCLCPP_ERROR(this->get_logger(), "Socket Error: %s", ex.what());
//            throw ex;
//        }
//
//
//        // tf2 广播 对坐标系进行转化
//        timeStamp_offset_ = declare_parameter("timestamp_offset", 0.0);
//        tf2_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
//
//
//        marker_pub_ = this->create_publisher<visualization_msgs::msg::Marker>("/aiming_point", 10);
//        detector_param_client_ = std::make_shared<rclcpp::AsyncParametersClient>(this, "armor_detector");
//        // 可视化发送信息
//        recv_pub_ = this->create_publisher<auto_aim_interfaces::msg::DebugRecvData>("/receive", 10);
//        latency_pub_ = this->create_publisher<std_msgs::msg::Float64>("/latency", 10);
//
//        // tracker reset service client
//        reset_tracker_client_ = this->create_client<std_srvs::srv::Trigger>("/tracker/reset");
//
//        aiming_point_.header.frame_id = "odom";
//        aiming_point_.ns = "aiming_point";
//        aiming_point_.type = visualization_msgs::msg::Marker::SPHERE;
//        aiming_point_.action = visualization_msgs::msg::Marker::ADD;
//        aiming_point_.scale.x = aiming_point_.scale.y = aiming_point_.scale.z = 0.12;
//        aiming_point_.color.r = 1.0;
//        aiming_point_.color.g = 1.0;
//        aiming_point_.color.b = 1.0;
//        aiming_point_.color.a = 1.0;
//        aiming_point_.lifetime = rclcpp::Duration::from_seconds(0.1);
//
//        // 自瞄 tracker 订阅
//        target_sub_ = this->create_subscription<auto_aim_interfaces::msg::Target>(
//                "/tracker/target", rclcpp::SensorDataQoS(),
//                std::bind(&VisionClient::sendGimbalData, this, std::placeholders::_1)
//        );
//
//        // 导航订阅
//        nav_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
//                "/cmd_vel", rclcpp::SensorDataQoS(),
//                std::bind(&VisionClient::twistSender, this, std::placeholders::_1)
//        );
//
//        receive_thread_ = std::thread(&VisionClient::receiveData, this);
//
//    }
//
//    VisionClient::~VisionClient() {
//
//        close(client_socket_fd);
//
//    }
//
//
//    void VisionClient::sendGimbalData(auto_aim_interfaces::msg::Target::SharedPtr target_msg) {
//
//        RCLCPP_INFO(get_logger(), "start send");
//        const static std::map<std::string, uint8_t> id_uint8_map{
//                {"",        0},
//                {"outpost", 0},
//                {"1",       1},
//                {"1",       1},
//                {"2",       2},
//                {"3",       3},
//                {"4",       4},
//                {"5",       5}
//        };
//
//        try {
//            SendGimbalPacket packet;
//
//            // 自瞄部分
//            packet.header = 0x5A;
//            packet.tracking = target_msg->tracking;
//            packet.id = id_uint8_map.at(target_msg->id);
//            packet.armors_num = target_msg->armors_num;
//            packet.x = target_msg->position.x;
//            packet.y = target_msg->position.y;
//            packet.z = target_msg->position.z;
//            packet.yaw = target_msg->yaw;
//            packet.vx = target_msg->velocity.x;
//            packet.vy = target_msg->velocity.y;
//            packet.vz = target_msg->velocity.z;
//            packet.v_yaw = target_msg->v_yaw;
//            packet.r1 = target_msg->radius_1;
//            packet.r2 = target_msg->radius_2;
//            packet.dz = target_msg->dz;
//
////            std::vector<uint8_t> send_data = toVectorGimbal(packet);
//
////
////            packet.header = 0x5A;
////            packet.tracking = target_msg->tracking;
////            packet.id = id_uint8_map.at(target_msg->id);
////            packet.armors_num = target_msg->armors_num;
////            packet.x = 11.0f;
////            packet.y = 12.0f;
////            packet.z = 13.0;
////            packet.yaw = 14.0f;
////            packet.vx = target_msg->velocity.x;
////            packet.vy = target_msg->velocity.y;
////            packet.vz = target_msg->velocity.z;
////            packet.v_yaw = target_msg->v_yaw;
////            packet.r1 = target_msg->radius_1;
////            packet.r2 = target_msg->radius_2;
////            packet.dz = target_msg->dz;
//
//
//
//
//            std_msgs::msg::Float64 latency;
//            latency.data = (this->now() - target_msg->header.stamp).seconds() * 1000.0f;
//            RCLCPP_DEBUG_STREAM(get_logger(), "Total latency: " + std::to_string(latency.data) + "ms");
//            latency_pub_->publish(latency);
//
//
//            // socket send
//            bzero(buffer_send, BUFFER_SIZE);
//            socklen_t serv_addr_len = sizeof (serv_addr_);
//
//            pack(packet, buffer_send);
//            RCLCPP_INFO(get_logger(), "size buffer_: %d", sizeof (buffer_send));
//            RCLCPP_INFO(get_logger(), "header %02x", (buffer_send)[0]);
//
//            int n_sendto = sendto(client_socket_fd,
//                                  (const char *)buffer_send,
//                                  sizeof (packet),
//                                  MSG_CONFIRM,
//                                  (struct sockaddr *) &serv_addr_,
//                                          serv_addr_len);
//
//
//            if (n_sendto < 0)
//                RCLCPP_ERROR(get_logger(), "Error Send");
//            else
//                RCLCPP_INFO(this->get_logger(), "Send data %ld to %s", ++send_data_cunt_, serv_ip_.c_str());
//
//        } catch (const std::exception &ex) {
//            RCLCPP_ERROR(get_logger(), "Error: %s", ex.what());
//        }
//
//    }
//
//    void VisionClient::receiveData() {
////        std::vector<uint8_t> recvData;
////        recvData.reserve(sizeof(ReceivePacket));
//        SendGimbalPacket packet;
//
//        bzero(buffer_send, BUFFER_SIZE);
//
//        packet.tracking = false;
//        packet.id = 0;
//        packet.armors_num = 0;
//        packet.x = 0.0f;
//        packet.y = 0.0f;
//        packet.z = 0.0f;
//        packet.yaw = 0.0f;
//        packet.vx = 0.0f;
//        packet.vy = 0.0f;
//        packet.vz = 0.0f;
//        packet.v_yaw = 0.0f;
//        packet.r1 = 0.0f;
//        packet.r2 = 0.0f;
//        packet.dz = 0.0f;
//
//        pack(packet, buffer_send);
//
//        sendto(client_socket_fd,
//                buffer_send,
//                0,
//                MSG_CONFIRM,
//                (const struct sockaddr *)&serv_addr_,
//                sizeof(serv_addr_));
//        RCLCPP_INFO(get_logger(), "connect init");
//
//
//
//        while (rclcpp::ok()) {
//            try {
//                RCLCPP_INFO(get_logger(), "setup receive node");
//                bzero(buffer_recv, BUFFER_SIZE);
//
//                socklen_t serv_addr_len = sizeof (serv_addr_);
//
//                RCLCPP_INFO(get_logger(), "recv_start\n");
//                int n_recvfrom = recvfrom(client_socket_fd,
//                                          buffer_recv,
//                                          BUFFER_SIZE,
//                                          MSG_WAITALL,
//                                          (struct sockaddr *)&serv_addr_,
//                                                  &serv_addr_len);
//
//
//
//                RCLCPP_INFO(get_logger(), "recv_finish\n");
////                RCLCPP_INFO(get_logger(), "%s", serv_addr_.sin_addr.s_addr);
////                RCLCPP_INFO(get_logger(), "%d", serv_addr_.sin_port);
//
//
//                if (n_recvfrom < 0)
//                    RCLCPP_ERROR(this->get_logger(), "Error reading from server");
//
//
//                ReceivePacket recvPacket;
//                unpack(recvPacket, buffer_recv);
//
//                // 电控数据可视化发布
//                recv_data_.aim_x = recvPacket.aim_x;
//                recv_data_.aim_y = recvPacket.aim_y;
//                recv_data_.aim_z = recvPacket.aim_z;
//                recv_data_.yaw = recvPacket.yaw;
//                recv_data_.pitch = -recvPacket.pitch;
//                recv_data_.roll = recvPacket.roll;
//                recv_data_.reset_tracker = recvPacket.reset_tracker;
//                recv_data_.reserved = recvPacket.reserved;
//                recv_data_.detect_color = recvPacket.detect_color;
//
////                RCLCPP_INFO(get_logger(), "%f %f %f %f %f %f %d %d %d",
////                            recv_data_.aim_x,
////                            recv_data_.aim_y,
////                            recv_data_.aim_z,
////                            recv_data_.yaw,
////                            recv_data_.pitch,
////                            recv_data_.roll,
////                            recv_data_.reset_tracker,
////                            recv_data_.reserved,
////                            recv_data_.detect_color
////                            );
//
//                recv_pub_->publish(recv_data_);
//
//
//                // 数据处理
//                if(recvPacket.detect_color != previous_detect_color_) {
//                    setParam(rclcpp::Parameter("detect_color", recv_data_.detect_color));
//                    previous_detect_color_ = recvPacket.detect_color;
//                }
//
//                if (recvPacket.reset_tracker)
//                    resetTracker();
//
//                // 电控读yaw, pitch, 对机器人状态进行tf2广播到ros的坐标
//                geometry_msgs::msg::TransformStamped t;
//                timeStamp_offset_ = this->get_parameter("timestamp_offset").as_double();
//                t.header.stamp = this->now() + rclcpp::Duration::from_seconds(timeStamp_offset_);
//                t.header.frame_id = "odom";
//                t.child_frame_id = "gimbal_link";
//                tf2::Quaternion q;
//                q.setRPY(recv_data_.roll, recv_data_.pitch, recv_data_.yaw);
//                t.transform.rotation = tf2::toMsg(q);
//                tf2_broadcaster_->sendTransform(t);
//
//
//                // 接受信息可视化
//                if (abs(recvPacket.aim_x) > 0.01) {
//                    aiming_point_.header.stamp = this->now();
//                    aiming_point_.pose.position.x = recvPacket.aim_x;
//                    aiming_point_.pose.position.y = recvPacket.aim_y;
//                    aiming_point_.pose.position.z = recvPacket.aim_z;
//                    marker_pub_->publish(aiming_point_);
//                }
//
//
//            } catch (const std::exception &ex) {
//                RCLCPP_ERROR(get_logger(), "Error receive data: %s", ex.what());
//            }
//
//
//        }
//    }
//
//
//    void VisionClient::twistSender(geometry_msgs::msg::Twist::SharedPtr nav_msg) {
//        try {
//            SendTwistPacket packet;
//
//            packet.header = 0x6A;
//            packet.linear_vx = nav_msg->linear.x;
//            packet.linear_vy = nav_msg->linear.y;
//            packet.angular = nav_msg->angular.z;
//            packet.v_yaw = nav_msg->angular.z;
//
//            // socket send
//            bzero(buffer_twist, BUFFER_SIZE);
//
////            RCLCPP_INFO(this->get_logger(), "linaer x: %f, %f %f", packet.linear_vx, packet.linear_vy,
////                        packet.linear_vz);
//
//            socklen_t serv_addr_len = sizeof(serv_addr_);
//
//            pack(packet, buffer_twist);
//
//
//            int n_sendto = sendto(
//                    client_socket_fd,
//                    (const char *)buffer_twist,
//                    sizeof(packet),
//                    MSG_CONFIRM,
//                    (struct sockaddr *) &serv_addr_,
//                    serv_addr_len);
//
//
//
//            perror("");
//            printf("%d", n_sendto);
//            if (n_sendto < 0)
//                RCLCPP_ERROR(get_logger(), "ERROR sending ");
//            else
//                RCLCPP_INFO(get_logger(), "Send data to %s", serv_ip_.c_str());
//
//        } catch (const std::exception &ex) {
//            RCLCPP_ERROR(get_logger(), "Error: %s", ex.what());
//        }
//    }
//
//
//    void VisionClient::resetTracker() {
//        if (!reset_tracker_client_->service_is_ready()) {
//            RCLCPP_WARN(get_logger(), "Service not ready, skipping tracker reset");
//            return;
//        }
//
//        auto request = std::make_shared<std_srvs::srv::Trigger::Request>();
//        reset_tracker_client_->async_send_request(request);
//        RCLCPP_INFO(get_logger(), "Reset tracker");
//    }
//
//    void VisionClient::setParam(const rclcpp::Parameter &param) {
//        if (!detector_param_client_->service_is_ready()) {
//            RCLCPP_WARN(get_logger(), "Service not ready, skipping parameter set");
//            return;
//        }
//        if (
//                !set_param_future_.valid() ||
//                set_param_future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
//            RCLCPP_INFO(get_logger(), "Setting detect_color to %ld...", param.as_int());
//            set_param_future_ = detector_param_client_->set_parameters(
//                    {param}, [this, param](const ResultFuturePtr &results) {
//                        for (const auto &result: results.get()) {
//                            if (!result.successful) {
//                                RCLCPP_ERROR(get_logger(), "Failed to set parameter: %s", result.reason.c_str());
//                                return;
//                            }
//                        }
//                        RCLCPP_INFO(get_logger(), "Successfully set detect_color to %ld!", param.as_int());
//                        initial_set_param_ = true;
//                    });
//        }
//
//
//    }
//}
//#include "rclcpp_components/register_node_macro.hpp"
//
//RCLCPP_COMPONENTS_REGISTER_NODE(nav_client::VisionClient)