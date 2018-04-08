SRCDIR	= ./
OBJDIR	= ./obj
BINDIR	= ./bin

CC		= g++
CFLAGS	= -std=c++11 -Wall -g -MMD -MP
LDFLAGS	= -lX11 -lEGL -lGLESv2
LIBS	=
INCS	= -I$(SRCDIR)
SRCS	= $(wildcard $(SRCDIR)/*.cpp)
OBJS	= $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
DEPENDS	= $(OBJS:.o=.d)
TARGET	= $(BINDIR)/XWindow.exe

all:			$(TARGET)

$(TARGET):		$(OBJS) $(LIBS)
				mkdir -p $(BINDIR)
				$(CC) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o:	$(SRCDIR)/%.cpp
				mkdir -p $(OBJDIR)
				$(CC) $(CFLAGS) $(INCS) -o $@ -c $<

clean:
		rm -fr $(TARGET) $(OBJDIR)

-include $(DEPENDS)
