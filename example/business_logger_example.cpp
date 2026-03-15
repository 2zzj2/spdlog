// business_logger_example.cpp
// 多业务日志系统使用示例

#include <spdlog/business_logger.h>
#include <iostream>
#include <thread>
#include <chrono>

using namespace spdlog;

// 模拟录制屏幕业务
void simulate_screen_recording() {
    auto logger = get_business_logger(BusinessType::ScreenRecord);
    
    logger->info("开始录制屏幕");
    logger->debug("设置录制分辨率为 1920x1080");
    logger->debug("设置帧率为 30fps");
    
    for (int i = 0; i < 5; ++i) {
        logger->info("录制中... 第 {} 秒", i + 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    logger->info("屏幕录制完成，保存文件: screen_capture_001.mp4");
}

// 模拟打开桌面业务
void simulate_desktop_open() {
    auto logger = get_business_logger(BusinessType::DesktopOpen);
    
    logger->info("打开桌面连接");
    logger->debug("目标主机: 192.168.1.100");
    logger->debug("连接端口: 3389");
    logger->info("桌面连接已建立");
    
    // 模拟一些操作
    logger->info("发送鼠标点击事件: (100, 200)");
    logger->info("发送键盘输入: 'Hello World'");
    
    logger->info("关闭桌面连接");
}

// 模拟录制键盘业务
void simulate_keyboard_recording() {
    auto logger = get_business_logger(BusinessType::KeyboardRecord);
    
    logger->info("开始录制键盘输入");
    
    // 模拟键盘事件
    logger->debug("捕获按键: Key_A");
    logger->debug("捕获按键: Key_B");
    logger->debug("捕获按键: Key_C");
    logger->warn("检测到特殊按键组合: Ctrl+C");
    logger->debug("捕获按键: Key_Return");
    
    logger->info("键盘录制结束，共捕获 5 个按键事件");
}

// 模拟录制声音业务
void simulate_sound_recording() {
    auto logger = get_business_logger(BusinessType::SoundRecord);
    
    logger->info("开始录制声音");
    logger->debug("音频格式: PCM, 44100Hz, 16bit, 立体声");
    logger->debug("输入设备: 麦克风 (Realtek Audio)");
    
    for (int i = 0; i < 3; ++i) {
        logger->info("录音中... 已录制 {} 秒", (i + 1) * 10);
    }
    
    logger->info("声音录制完成，保存文件: audio_recording_001.wav");
}

// 演示使用全局便捷函数
void demo_global_functions() {
    std::cout << "\n=== 使用全局便捷函数 ===" << std::endl;
    
    // 使用全局便捷函数记录不同业务的日志
    business_info(BusinessType::ScreenRecord, "【全局函数】屏幕录制服务启动");
    business_info(BusinessType::DesktopOpen, "【全局函数】桌面连接服务启动");
    business_info(BusinessType::KeyboardRecord, "【全局函数】键盘录制服务启动");
    business_info(BusinessType::SoundRecord, "【全局函数】声音录制服务启动");
    
    // 记录不同级别的日志
    business_debug(BusinessType::ScreenRecord, "【全局函数】调试信息: 初始化完成");
    business_warn(BusinessType::DesktopOpen, "【全局函数】警告: 连接延迟较高");
    business_error(BusinessType::KeyboardRecord, "【全局函数】错误: 无法访问键盘设备");
}

// 演示日志级别控制
void demo_log_level_control() {
    std::cout << "\n=== 日志级别控制演示 ===" << std::endl;
    
    auto screen_logger = get_business_logger(BusinessType::ScreenRecord);
    
    std::cout << "设置屏幕录制日志级别为 warning" << std::endl;
    business_logger_manager::instance().set_level(BusinessType::ScreenRecord, level::warn);
    
    // 这些日志不会输出，因为级别低于 warning
    screen_logger->info("这条信息不会显示 (info级别)");
    screen_logger->debug("这条调试信息不会显示 (debug级别)");
    
    // 这些日志会输出
    screen_logger->warn("这是一条警告信息");
    screen_logger->error("这是一条错误信息");
    
    // 恢复为info级别
    business_logger_manager::instance().set_level(BusinessType::ScreenRecord, level::info);
    screen_logger->info("日志级别已恢复为 info");
}

// 演示大文件写入（测试轮转功能）
void demo_rotation_feature() {
    std::cout << "\n=== 文件轮转功能演示 ===" << std::endl;
    std::cout << "正在写入大量数据以触发文件轮转..." << std::endl;
    
    auto logger = get_business_logger(BusinessType::ScreenRecord);
    
    // 写入足够多的数据以触发轮转
    // 每个文件30M，最多3个文件
    std::string large_msg(1024, 'X'); // 1KB的消息
    
    for (int i = 0; i < 1000; ++i) {
        logger->info("轮转测试消息 {}: {}", i, large_msg);
    }
    
    std::cout << "数据写入完成，检查 logs/screen_record.log* 文件" << std::endl;
}

int main() {
    try {
        std::cout << "=== 多业务日志系统示例 ===" << std::endl;
        
        // 1. 初始化业务日志管理器
        business_logger_manager::config cfg;
        cfg.log_dir = "logs";                           // 日志目录
        cfg.max_file_size = 30 * 1024 * 1024;          // 单个文件最大30M
        cfg.max_files = 3;                              // 最多保留3个文件
        cfg.level = level::debug;                       // 默认日志级别为debug
        
        business_logger_init(cfg);
        
        std::cout << "日志目录: " << cfg.log_dir << std::endl;
        std::cout << "最大文件大小: " << cfg.max_file_size / (1024 * 1024) << " MB" << std::endl;
        std::cout << "最大文件数: " << cfg.max_files << std::endl;
        
        // 2. 模拟各业务操作
        std::cout << "\n=== 录制屏幕业务 ===" << std::endl;
        simulate_screen_recording();
        
        std::cout << "\n=== 打开桌面业务 ===" << std::endl;
        simulate_desktop_open();
        
        std::cout << "\n=== 录制键盘业务 ===" << std::endl;
        simulate_keyboard_recording();
        
        std::cout << "\n=== 录制声音业务 ===" << std::endl;
        simulate_sound_recording();
        
        // 3. 演示其他功能
        demo_global_functions();
        demo_log_level_control();
        
        // 4. 可选：演示文件轮转（会写入大量数据）
        // demo_rotation_feature();
        
        // 5. 刷新所有日志
        business_logger_manager::instance().flush_all();
        
        std::cout << "\n=== 示例结束 ===" << std::endl;
        std::cout << "请检查 logs/ 目录下的日志文件:" << std::endl;
        std::cout << "  - screen_record.log" << std::endl;
        std::cout << "  - desktop_open.log" << std::endl;
        std::cout << "  - keyboard_record.log" << std::endl;
        std::cout << "  - sound_record.log" << std::endl;
        
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "日志系统错误: " << ex.what() << std::endl;
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "错误: " << ex.what() << std::endl;
        return 1;
    }
    
    return 0;
}
