#include "spdlog/business_logger.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    try {
        business_log::BusinessLogger::instance().init("business_logs");

        business_log::BusinessLogger::instance().info(business_log::BusinessType::ScreenRecording, "开始录制屏幕");
        business_log::BusinessLogger::instance().debug(business_log::BusinessType::ScreenRecording, "初始化屏幕捕获设备");
        
        business_log::BusinessLogger::instance().info(business_log::BusinessType::DesktopOpen, "用户打开桌面");
        business_log::BusinessLogger::instance().warn(business_log::BusinessType::DesktopOpen, "桌面分辨率检测异常");

        business_log::BusinessLogger::instance().info(business_log::BusinessType::KeyboardRecording, "开始录制键盘");
        business_log::BusinessLogger::instance().debug(business_log::BusinessType::KeyboardRecording, "键盘输入捕获线程启动");

        business_log::BusinessLogger::instance().info(business_log::BusinessType::AudioRecording, "开始录制声音");
        business_log::BusinessLogger::instance().error(business_log::BusinessType::AudioRecording, "音频设备初始化失败");

        for (int i = 0; i < 5; i++) {
            business_log::BusinessLogger::instance().info(business_log::BusinessType::ScreenRecording, "录制进度: {}%", i * 20);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        business_log::BusinessLogger::instance().info(business_log::BusinessType::ScreenRecording, "屏幕录制完成");
        business_log::BusinessLogger::instance().info(business_log::BusinessType::DesktopOpen, "桌面会话结束");
        business_log::BusinessLogger::instance().info(business_log::BusinessType::KeyboardRecording, "键盘录制停止");
        business_log::BusinessLogger::instance().info(business_log::BusinessType::AudioRecording, "音频录制停止");

        spdlog::shutdown();
        std::cout << "日志示例运行完成！日志文件保存在 business_logs 目录中" << std::endl;
        
    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "日志初始化失败: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
