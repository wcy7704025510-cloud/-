# 编译器
CXX = g++

# 编译选项
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

# 头文件路径
INCLUDES = -I. \
           -Icore \
           -Iinfrastructure/dbapi \
           -Iinfrastructure/netapi \
           -Iinfrastructure/netapi/common \
           -Iinfrastructure/netapi/mediator \
           -Iinfrastructure/netapi/net \
           -Ilogic \
           -Iutils

# 所有 cpp 源文件
SRC = $(wildcard core/*.cpp) \
      $(wildcard infrastructure/dbapi/*.cpp) \
      $(wildcard infrastructure/netapi/mediator/*.cpp) \
      $(wildcard infrastructure/netapi/net/*.cpp) \
      $(wildcard logic/*.cpp) \
      $(wildcard utils/*.cpp)

# 目标文件
OBJ = $(SRC:.cpp=.o)

# 输出程序
TARGET = release/HouseServer

# ====================== 关键在这里 ======================
# 链接：线程库 + MySQL库
LIBS = -pthread -lmysqlclient
# ========================================================

# 编译
all: $(TARGET)

$(TARGET): $(OBJ)
	mkdir -p release
	$(CXX) $(OBJ) -o $@ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# 清理
clean:
	rm -f $(OBJ) $(TARGET)
