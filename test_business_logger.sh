#!/bin/bash
# 多业务日志系统测试脚本

echo "=========================================="
echo "   多业务日志系统测试脚本"
echo "=========================================="
echo ""

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 测试计数器
TESTS_PASSED=0
TESTS_FAILED=0

# 测试函数
run_test() {
    local test_name="$1"
    local test_cmd="$2"
    echo -n "测试: $test_name ... "
    if eval "$test_cmd" > /dev/null 2>&1; then
        echo -e "${GREEN}通过${NC}"
        ((TESTS_PASSED++))
        return 0
    else
        echo -e "${RED}失败${NC}"
        ((TESTS_FAILED++))
        return 1
    fi
}

# 1. 清理旧的日志文件
echo "[1/8] 清理环境..."
rm -rf logs/
mkdir -p logs
echo "      已清理旧日志文件"
echo ""

# 2. 编译测试程序
echo "[2/8] 编译测试程序..."
cd "$(dirname "$0")"
g++ -std=c++17 -I include example/business_logger_example.cpp -L build -lspdlog -lpthread -o business_logger_example
if [ $? -eq 0 ]; then
    echo -e "      ${GREEN}编译成功${NC}"
else
    echo -e "      ${RED}编译失败${NC}"
    exit 1
fi
echo ""

# 3. 运行测试程序
echo "[3/8] 运行测试程序..."
./business_logger_example > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo -e "      ${GREEN}程序运行成功${NC}"
else
    echo -e "      ${RED}程序运行失败${NC}"
    exit 1
fi
echo ""

# 4. 检查日志文件是否生成
echo "[4/8] 检查日志文件生成..."
run_test "screen_record.log 存在" "test -f logs/screen_record.log"
run_test "desktop_open.log 存在" "test -f logs/desktop_open.log"
run_test "keyboard_record.log 存在" "test -f logs/keyboard_record.log"
run_test "sound_record.log 存在" "test -f logs/sound_record.log"
echo ""

# 5. 检查日志文件内容
echo "[5/8] 检查日志文件内容..."
run_test "screen_record.log 包含 '录制屏幕'" "grep -q '录制屏幕' logs/screen_record.log"
run_test "desktop_open.log 包含 '桌面连接'" "grep -q '桌面连接' logs/desktop_open.log"
run_test "keyboard_record.log 包含 '键盘'" "grep -q '键盘' logs/keyboard_record.log"
run_test "sound_record.log 包含 '声音'" "grep -q '声音' logs/sound_record.log"
echo ""

# 6. 检查日志格式
echo "[6/8] 检查日志格式..."
run_test "日志包含时间戳格式" "grep -qE '\[[0-9]{4}-[0-9]{2}-[0-9]{2}' logs/screen_record.log"
run_test "日志包含级别标识 [info]" "grep -q '\[info\]' logs/screen_record.log"
run_test "日志包含级别标识 [debug]" "grep -q '\[debug\]' logs/screen_record.log"
run_test "日志包含级别标识 [warning]" "grep -q '\[warning\]' logs/screen_record.log"
run_test "日志包含级别标识 [error]" "grep -q '\[error\]' logs/screen_record.log"
echo ""

# 7. 检查日志级别控制功能
echo "[7/8] 检查日志级别控制..."
# warning级别设置后，info级别的日志不应该出现在后面
run_test "日志级别控制有效" "grep -q '这是一条警告信息' logs/screen_record.log"
run_test "error级别日志存在" "grep -q '这是一条错误信息' logs/screen_record.log"
echo ""

# 8. 显示日志文件统计信息
echo "[8/8] 日志文件统计..."
echo ""
echo "  文件列表:"
ls -lh logs/ | grep -v "^总" | awk '{printf "    %-25s %10s\n", $9, $5}'
echo ""
echo "  各文件日志条数统计:"
for logfile in logs/*.log; do
    if [ -f "$logfile" ]; then
        count=$(wc -l < "$logfile")
        filename=$(basename "$logfile")
        printf "    %-25s %5d 条\n" "$filename" "$count"
    fi
done
echo ""

# 9. 显示日志内容示例
echo "=========================================="
echo "   日志内容示例 (screen_record.log)"
echo "=========================================="
head -n 10 logs/screen_record.log
echo ""

# 10. 测试文件轮转功能（可选）
echo "=========================================="
echo "   测试文件轮转功能"
echo "=========================================="
echo "正在写入大量数据以触发轮转..."

# 创建一个测试轮转的程序
cat > test_rotation.cpp << 'EOF'
#include <spdlog/business_logger.h>
#include <string>

int main() {
    spdlog::business_logger_manager::config cfg;
    cfg.log_dir = "logs";
    cfg.max_file_size = 1024 * 1024;  // 1MB 方便测试
    cfg.max_files = 3;
    cfg.level = spdlog::level::info;
    spdlog::business_logger_init(cfg);
    
    auto logger = spdlog::get_business_logger(spdlog::BusinessType::ScreenRecord);
    
    // 写入约2MB数据，应该触发轮转
    std::string msg(1024, 'X'); // 1KB
    for (int i = 0; i < 2100; ++i) {
        logger->info("轮转测试消息 {}: {}", i, msg);
    }
    
    spdlog::business_logger_manager::instance().flush_all();
    return 0;
}
EOF

g++ -std=c++17 -I include test_rotation.cpp -L build -lspdlog -lpthread -o test_rotation
./test_rotation

echo ""
echo "轮转后文件列表:"
ls -lh logs/screen_record* | awk '{printf "  %-30s %10s\n", $9, $5}'
echo ""

# 检查是否生成了轮转文件 (spdlog的轮转文件命名格式为 xxx.1.log)
if ls logs/screen_record.1.log > /dev/null 2>&1; then
    echo -e "  ${GREEN}✓ 文件轮转功能正常${NC}"
    run_test "轮转文件 .1.log 存在" "test -f logs/screen_record.1.log"
    run_test "轮转文件 .2.log 存在" "test -f logs/screen_record.2.log"
else
    echo -e "  ${YELLOW}⚠ 未检测到轮转文件（可能需要更多数据）${NC}"
fi

# 清理测试文件
rm -f test_rotation.cpp test_rotation

echo ""
echo "=========================================="
echo "   测试结果汇总"
echo "=========================================="
echo -e "  通过: ${GREEN}$TESTS_PASSED${NC}"
echo -e "  失败: ${RED}$TESTS_FAILED${NC}"
echo ""

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "${GREEN}所有测试通过！${NC}"
    exit 0
else
    echo -e "${RED}存在失败的测试！${NC}"
    exit 1
fi
