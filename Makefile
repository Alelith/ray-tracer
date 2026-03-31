NAME = ray_tracer

#Colours
DEF_COLOR = \033[0;39m
GRAY = \033[0;90m
RED = \033[0;91m
GREEN = \033[0;92m
YELLOW = \033[0;93m
BLUE = \033[0;94m
MAGENTA = \033[0;95m
CYAN = \033[0;96m
WHITE = \033[0;97m

INCLUDE_DIRS = include/ include/geometry/ include/maths/ include/render/ include/debug/
INCLUDES = $(addprefix -I , $(INCLUDE_DIRS))
CXX = c++ -g
CXXFLAGS = -Wall -Wextra -Werror -std=c++17
LDFLAGS = -lm

SRC_DIR = src/
OBJ_DIR = obj/

GEOMETRY_DIR = geometry/
GEOMETRY_FILES = Geometry Sphere Transform Limits AABB LinearBVH

MATHS_DIR = maths/
MATHS_FILES = BhaskaraSolver CardanoSolver FerrariSolver LinearSolver \
				Matrix4x4 Point3 Random Ray Solver Vector3

RENDER_DIR = render/
RENDER_FILES = Camera Material Texture

DEBUG_DIR = debug/
DEBUG_FILES = Timer

MAIN = main

SRC_FILES += $(MAIN)
SRC_FILES += $(addprefix $(GEOMETRY_DIR), $(GEOMETRY_FILES))
SRC_FILES += $(addprefix $(MATHS_DIR), $(MATHS_FILES))
SRC_FILES += $(addprefix $(RENDER_DIR), $(RENDER_FILES))
SRC_FILES += $(addprefix $(DEBUG_DIR), $(DEBUG_FILES))
SRCS = $(addprefix $(SRC_DIR), $(addsuffix .cpp, $(SRC_FILES)))
OBJS = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(SRC_FILES)))
OBJSF = .cache_exists

all: $(NAME)

$(OBJSF):
	@echo -e "$(MAGENTA)Creating object directory$(DEF_COLOR)"
	@mkdir -p $(OBJ_DIR)
	@touch $@

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) $(INCLUDES) -o $(NAME) $(LDFLAGS)
	@echo -e "$(GREEN)$(NAME) compiled!$(DEF_COLOR)"

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp | $(OBJSF)
	@echo -e "$(CYAN)Compiling: $<$(DEF_COLOR)"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)

clean:
	@echo -e "$(RED)Removing .o object files...$(DEF_COLOR)"
	@rm -rf $(OBJ_DIR)
	@rm -f $(OBJSF)

fclean: clean
	@echo -e "$(RED)Removing $(NAME)...$(DEF_COLOR)"
	@rm -f $(NAME)

re: clean all

.PHONY: all clean fclean re
