CXX      := c++
CXXFLAGS := -std=c++17 -Wall -Wextra -I../include -I../include/maths

OBJ_DIR := obj

# Source files from the main project needed by the solvers
SOLVER_SRCS := \
	../src/maths/Solver.cpp        \
	../src/maths/LinearSolver.cpp  \
	../src/maths/BhaskaraSolver.cpp \
	../src/maths/CardanoSolver.cpp  \
	../src/maths/FerrariSolver.cpp  \
	../src/maths/Ray.cpp            \
	../src/maths/Vector3.cpp

# Test source files
TEST_SRCS := \
	test_main.cpp              \
	test_linear_solver.cpp     \
	test_bhaskara_solver.cpp   \
	test_cardano_solver.cpp    \
	test_ferrari_solver.cpp

# Object files: solver srcs  →  obj/src/maths/*.o
#               test srcs    →  obj/test/*.o
SOLVER_OBJS := $(patsubst ../%.cpp, $(OBJ_DIR)/%.o, $(SOLVER_SRCS))
TEST_OBJS   := $(patsubst %.cpp,    $(OBJ_DIR)/test/%.o, $(TEST_SRCS))
OBJS        := $(SOLVER_OBJS) $(TEST_OBJS)

TARGET := run_tests

.PHONY: all clean fclean re run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Solver object files  (source lives one level up: ../src/maths/)
$(OBJ_DIR)/src/maths/%.o: ../src/maths/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Test object files  (source in current directory)
$(OBJ_DIR)/test/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(TARGET)

re: fclean all
