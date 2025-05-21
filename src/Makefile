BUILD ?= debug
TARGET := test_bigint
CXX := g++
CXXFLAGS := -std=c++23
LDFLAGS :=
CXXFLAGS += -MMD -MP
CXXFLAGS += -fstrict-aliasing -Wstrict-aliasing \
            -Wold-style-cast
# 预处理标志
CPPFLAGS :=

ifeq ($(BUILD), debug)
# 为了获取完整的栈回溯
CXXFLAGS += -g -fno-omit-frame-pointer
# AddressSanitizer 是 LeakSanitizer 的超集。
# 检测内存的 LeakSanitizer 是集成在 AddressSanitizer 中的一个相对独立的工具，它
# 工作在检查过程的最后阶段。
CXXFLAGS += -fsanitize=address
LDFLAGS += -static-libasan -fsanitize=address
# UBSanitizer 也变成运行时检查
CXXFLAGS += -fsanitize=undefined
LDFLAGS += -fsanitize=undefined
else
# assert() 开关
CXXFLAGS += -DNDEBUG
endif

SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)
ASMS := $(SRCS:.cpp=.s)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean insight

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ -MF $*.d

-include $(DEPS)

insight: $(SRCS)
	$(CXX) $(CXXFLAGS) -S -fverbose-asm $^

clean:
	rm -rf $(TARGET) $(OBJS) $(ASMS) $(DEPS)
