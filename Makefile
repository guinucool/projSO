# Opções do compilador
CC = gcc
CFLAGS = -Wall -g -c

# Opções de diretorias
SRCDIR = src
TOOLSDIR = tools

# Diretorias de output
OUTDIR = obj
BINDIR = bin

# Variáveis de compilação
SRCS = $(wildcard $(SRCDIR)/*.c $(SRCDIR)/$(TOOLSDIR)/*.c)
SRCS:= $(patsubst $(SRCDIR)/$(TOOLSDIR)/%.c, $(SRCDIR)/%.c, $(SRCS))

OBJS = $(patsubst $(SRCDIR)/%.c, $(OUTDIR)/%.o, $(SRCS))

MONITOR = $(filter-out $(OUTDIR)/tracer.o ,$(OBJS))
TRACER = $(filter-out $(OUTDIR)/monitor.o ,$(OBJS))

# Compilação
all: folders server client

server: $(BINDIR)/monitor

client: $(BINDIR)/tracer

folders:
	mkdir -p src tmp $(OUTDIR) $(BINDIR)

$(BINDIR)/monitor: $(MONITOR)
	gcc -g $^ -o $@

$(BINDIR)/tracer: $(TRACER)
	gcc -g $^ -o $@

$(OUTDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $^ -o $@

$(OUTDIR)/%.o: $(SRCDIR)/$(TOOLSDIR)/%.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f obj/* tmp/* bin/tracer bin/monitor