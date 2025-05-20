# 做什么？
# g++ -o server -g 1.cpp 2.cpp 
# -I... -L... -lmuduo_net -lmuduo_base -lpthread
cmake_minimum_required(VERSION 3.0)
project(myMuduo)

# 配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 设置可执行文件生成的路径(build/../bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 配置头文件搜索路径

# 配置库文件搜索路径

# 设置需要编译的源文件列表
set(SRC_LIST muduo_server.cpp)

# # 把指定路径下的所有源文件名字放入变量名SRC_LIST
# aux_source_directory(. SRC_LIST)

add_executable(server ${SRC_LIST})

# server需要链接后面几个文件
target_link_libraries(server muduo_net muduo_base pthread)
