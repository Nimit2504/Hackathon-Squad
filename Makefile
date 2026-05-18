CXX      = g++
CXXFLAGS = -O2 -std=c++17 -Wall -Wextra -Wshadow
TARGET   = hackathon_squad

# ── Build ─────────────────────────────────────────────────────────────────────
all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp

# ── Run individual tests ──────────────────────────────────────────────────────
test1: $(TARGET)
	@echo "=== Test 1 (small, N=5) ==="
	@./$(TARGET) < tests/test1.txt
	@echo "--- expected ---"
	@cat tests/test1_expected.txt

test2: $(TARGET)
	@echo "=== Test 2 (cycle C7, N=7) ==="
	@./$(TARGET) < tests/test2.txt
	@echo "--- expected ---"
	@cat tests/test2_expected.txt

test3: $(TARGET)
	@echo "=== Test 3 (single node, N=1) ==="
	@./$(TARGET) < tests/test3.txt
	@echo "--- expected ---"
	@cat tests/test3_expected.txt

test4: $(TARGET)
	@echo "=== Test 4 (complete graph K4, N=4) ==="
	@./$(TARGET) < tests/test4.txt
	@echo "--- expected ---"
	@cat tests/test4_expected.txt

# Run all tests sequentially
tests: test1 test2 test3 test4

# ── Generate a large random test and time it ─────────────────────────────────
stress: $(TARGET)
	python3 tests/gen.py 200000 0.000005 42 > /tmp/big_test.txt
	time ./$(TARGET) < /tmp/big_test.txt | head -c 200

# ── Cleanup ───────────────────────────────────────────────────────────────────
clean:
	rm -f $(TARGET) $(TARGET).exe /tmp/big_test.txt

.PHONY: all tests test1 test2 test3 test4 stress clean
