# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cgoldens <cgoldens@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/10 14:18:55 by cgoldens          #+#    #+#              #
#    Updated: 2026/01/30 11:59:26 by cgoldens         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

SRCDIR = ./srcs/
INCDIR = ./includes/
OBJDIR = ./build/

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -rf

SRCS = $(shell find $(SRCDIR) -type f -name "*.cpp")
OBJS = $(SRCS:$(SRCDIR)%.cpp=$(OBJDIR)%.o)

GREEN = \033[1;32m
CYAN = \033[1;36m
YELLOW = \033[1;33m
RESET = \033[0m

# Règles
all: header $(NAME)

header:
	@echo "$(GREEN)"
	@echo " _       __     __                        "
	@echo "| |     / /__  / /_  ________  ______   __"
	@echo "| | /| / / _ \\/ __ \\/ ___/ _ \\/ ___/ | / /"
	@echo "| |/ |/ /  __/ /_/ (__  )  __/ /   | |/ / "
	@echo "|__/|__/\\___/_.___/____/\\___/_/    |___/  "
	@echo "                                          "
	@echo "BY CGOLDENS, NADAHMAN, YAOBERSO"
	@echo "$(RESET)"

${OBJDIR}%.o: $(SRCDIR)%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -I$(INCDIR) -c -o $@ $<

$(NAME): $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(CYAN)Build completed!$(RESET)"

clean:
	@echo "$(YELLOW)Cleaning object files...$(RESET)"
	$(RM) $(OBJS)
	$(RM) $(OBJDIR)

fclean: clean
	@echo "$(YELLOW)Cleaning $(NAME) binary$(RESET)"
	$(RM) $(NAME)


re: fclean all

.PHONY: all clean fclean re