CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

TARGET = sysmon_app

SRCS = main.cpp \
       sysmon_service.cpp \
       cpu_collector.cpp \
       mem_collector.cpp \
       logger.cpp\
       config.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)