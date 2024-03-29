//
// Created by shuxy on 24-3-8.
//

#ifndef BUILD_NAV_PACKET_HPP
#define BUILD_NAV_PACKET_HPP

#include "algorithm"
#include "cstdint"
#include "vector"

namespace nav_client{


    struct CmdVelData{
        // 线速度 m/s
        float linear_vx;
        float linear_vy;
        float linear_vz;
        // 旋转角速度 rad/s
        float angular_vx;
        float angular_vy;
        float angluar_vz;
        // 欧拉角
        float v_yaw;
        float v_roll;
        float v_pitch;
    };

    struct ReceivePacket{

        uint8_t header = 0xA6;
        float roll;
        float pitch;
        float yaw;
        uint8_t gameStatus;
        // 比赛情况

    }__attribute__((packed));


    struct SendGimbalPacket{

        /*      自瞄部分     */
        uint8_t header = 0x5A ;
        bool tracking;
        uint8_t  id;
        uint8_t armors_num;
        uint8_t reserved;
        float x;
        float y;
        float z;
        float yaw;
        float vx;
        float vy;
        float vz;
        float v_yaw;
        float r1;
        float r2;
        float dz;
        /*  决策部分   */

    }__attribute__((packed));


//    struct Robot_set
//    {
//        uint8_t header;
//        /** chassis_control **/
//        fp32 vx_set = 0.f;
//        fp32 vy_set = 0.f;
//        fp32 wz_set = 0.f;
//
//        /** gimbal_control **/
//        fp32 yaw_set = 0.f;
//        fp32 pitch_set = 0.f;
//
//        /** IMU **/
//        fp32 ins_yaw = 0.f;
//        fp32 ins_pitch = 0.f;
//        fp32 ins_roll = 0.f;
//        fp32 ins_yaw_v = 0.f;
//        fp32 ins_pitch_v = 0.f;
//        fp32 ins_roll_v = 0.f;
//
//        /** other **/
//        fp32 yaw_relative = 0.f;
//        fp32 pitch_relative = 0.f;
//        Types::ROBOT_MODE mode = Types::ROBOT_MODE::ROBOT_NO_FORCE;
//    } __attribute__((packed));
//

    struct SendTwistPacket {
        /*       导航部分       */
        uint8_t header = 0x6A;
        // 线速度 m/s
        float linear_vx;
        float linear_vy;
        float angular;
        // 旋转角速度 rad/s
        float v_yaw;

    }__attribute__((packed));
//
    inline ReceivePacket fromVector(const std::vector<uint8_t> & data){
        ReceivePacket packet;
        std::copy(data.begin(), data.end(), reinterpret_cast<uint8_t *>(&packet));
        return packet;
    }

    inline std::vector<uint8_t> toVectorGimbal(const SendGimbalPacket &data){
        std::vector<uint8_t> packet(sizeof(SendGimbalPacket));
        std::copy(
                reinterpret_cast<const uint8_t *>(&data),
                reinterpret_cast<const uint8_t *>(&data) + sizeof (SendGimbalPacket), packet.begin()
                );
        return packet;
    }

    inline  std::vector<uint8_t> toVectorTwist(const SendTwistPacket &data) {
        std::vector<uint8_t> packet(sizeof(SendTwistPacket));
        std::copy(
                reinterpret_cast<const uint8_t *>(&data),
                reinterpret_cast<const uint8_t *>(&data) + sizeof(SendTwistPacket), packet.begin()
        );
        return packet;
    }

    inline void pack(const SendTwistPacket &data, char* buffer_){
        for (size_t i =0; i < sizeof (data); i++)
            buffer_[i] = *((uint8_t *)&data + i);

    }

    inline void pack(const SendGimbalPacket &data, char* buffer_){
        for (size_t i = 0; i < sizeof (data); i++)
            buffer_[i] = *((uint8_t *)&data + i);
    }

    inline void unpack(const ReceivePacket &data, char* buffer_){
        for (size_t i  = 0 ; i < sizeof (data); i++)
            *((uint8_t *)&data  + i) = buffer_[i];
    }




}

#endif //BUILD_NAV_PACKET_HPP
