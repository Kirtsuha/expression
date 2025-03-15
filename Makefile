CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -g
SRCDIR = src
TESTDIR = tests
BINDIR = bin

# Цели
all: $(BINDIR)/differentiator

# Исполняемый файл для программы
$(BINDIR)/differentiator: $(SRCDIR)/main.o $(SRCDIR)/expression.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Исполняемый файл для тестов
$(TESTDIR)/test_expression: $(TESTDIR)/test_expression.o $(SRCDIR)/expression.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Правила для создания .o файлов
$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TESTDIR)/%.o: $(TESTDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Тестирование
test: $(TESTDIR)/test_expression
	$(TESTDIR)/test_expression

# Очистка
clean:
	rm -f $(SRCDIR)/*.o $(TESTDIR)/*.o $(BINDIR)/differentiator $(TESTDIR)/test_expression