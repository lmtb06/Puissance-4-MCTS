# Makefile pour la compilation du jeu

# Variables de base
TARGET  := jeu
CC      := gcc
CFLAGS  := -Wall -Wextra -g
LDFLAGS := -lm
OBJDIR  := obj
LIBDIR  := lib
SRCS    := fonctions.c main.c
OBJS    := $(SRCS:%.c=$(OBJDIR)/%.o)
LIBNAME := libjeu.so

# Cible principale
all: directories $(TARGET)

# Création des répertoires
directories:
	@mkdir -p $(OBJDIR) $(LIBDIR)

# Compilation de l'exécutable
$(TARGET): $(OBJS)
	@echo "Linking $@"
	@$(CC) $^ -o $@ $(LDFLAGS)

# Compilation des fichiers sources
$(OBJDIR)/%.o: %.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -fPIC -c $< -o $@

# Compilation avec optimisation -O3
optim: CFLAGS += -O3
optim: directories $(TARGET)

# Création de la bibliothèque partagée
lib: $(LIBDIR)/$(LIBNAME)

$(LIBDIR)/$(LIBNAME): $(OBJS)
	@echo "Creating shared library $@"
	@$(CC) -shared $^ -o $@ $(LDFLAGS)

# Nettoyage du projet
clean:
	@echo "Cleaning up"
	@rm -rf $(OBJDIR) $(LIBDIR) $(TARGET)

# Empêcher make de confondre les noms de cibles avec des fichiers
.PHONY: all clean lib directories