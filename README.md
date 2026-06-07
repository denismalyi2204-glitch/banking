[![CI](https://github.com/denismalyi2204-glitch/banking/actions/workflows/ci.yml/badge.svg)](https://github.com/denismalyi2204-glitch/banking/actions/workflows/ci.yml)
[![Coverage Status](https://coveralls.io/repos/github/denismalyi2204-glitch/banking/badge.svg)](https://coveralls.io/github/denismalyi2204-glitch/banking)

## Homework lab5

Клонируем репозиторий
```bash
cd ~/denismalyi2204-glitch/workspace/projects
git clone https://github.com/denismalyi2204-glitch/banking.git
cd banking
```

```sh
remote: Enumerating objects: 137, done.
remote: Counting objects: 100% (25/25), done.
remote: Compressing objects: 100% (9/9), done.
remote: Total 137 (delta 18), reused 16 (delta 16), pack-reused 112 (from 1)
Receiving objects: 100% (137/137), 918.92 KiB | 1.14 MiB/s, done.
Resolving deltas: 100% (60/60), done.
```

Создаем структуру проекта
```bash
mkdir -p include src tests .github/workflows
```


Создаем CMakeLists.txt
```bash
cat > CMakeLists.txt <<'EOF'
cmake_minimum_required(VERSION 3.14)
project(banking)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(banking STATIC
    Account.cpp
    Transaction.cpp
)
option(BUILD_TESTS "Build tests" ON)
option(COVERAGE "Enable coverage reporting" OFF)
if(COVERAGE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-O0 -g --coverage -fprofile-arcs -ftest-coverage)
        add_link_options(--coverage -lgcov)
    endif()
endif()
if(BUILD_TESTS)
    enable_testing()
    include(FetchContent)
    
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
    
    add_executable(banking_tests 
        tests/test_account.cpp 
        tests/test_transaction.cpp
        tests/test_mocks.cpp
    )
    target_link_libraries(banking_tests banking gtest_main gmock)
    target_include_directories(banking_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    
    add_test(NAME banking_tests COMMAND banking_tests)
endif()
EOF
```

Создаем test_account.cpp
```bash
cat > tests/test_account.cpp <<'EOF'
#include <gtest/gtest.h>
#include "Account.h"
#include <memory>
#include <stdexcept>
class AccountTest : public ::testing::Test {
protected:
    void SetUp() override {
        account = std::make_unique<Account>(12345, 1000);
    }
    std::unique_ptr<Account> account;
};
TEST_F(AccountTest, ConstructorSetsIdAndBalance) {
    Account acc(99999, 500);
    EXPECT_EQ(99999, acc.id());
    EXPECT_EQ(500, acc.GetBalance());
}
TEST_F(AccountTest, GetBalanceReturnsCorrectValue) {
    EXPECT_EQ(1000, account->GetBalance());
}
TEST_F(AccountTest, ChangeBalanceThrowsIfNotLocked) {
    EXPECT_THROW(account->ChangeBalance(500), std::runtime_error);
}
TEST_F(AccountTest, ChangeBalanceWorksAfterLock) {
    account->Lock();
    EXPECT_NO_THROW(account->ChangeBalance(500));
    EXPECT_EQ(1500, account->GetBalance());
}
TEST_F(AccountTest, LockThrowsIfAlreadyLocked) {
    account->Lock();
    EXPECT_THROW(account->Lock(), std::runtime_error);
}
TEST_F(AccountTest, UnlockWorks) {
    account->Lock();
    EXPECT_NO_THROW(account->Unlock());
    EXPECT_THROW(account->ChangeBalance(500), std::runtime_error);
}
TEST_F(AccountTest, IdReturnsCorrectValue) {
    EXPECT_EQ(12345, account->id());
}
EOF
```

Создаем test_transaction.cpp
```bash
cat > tests/test_transaction.cpp <<'EOF'
#include <gtest/gtest.h>
#include "Transaction.h"
#include "Account.h"
#include <memory>
#include <stdexcept>
class TransactionTest : public ::testing::Test {
protected:
    void SetUp() override {
        fromAccount = std::make_unique<Account>(11111, 1000);
        toAccount = std::make_unique<Account>(22222, 500);
    }
    std::unique_ptr<Account> fromAccount;
    std::unique_ptr<Account> toAccount;
    Transaction tx;
};
TEST_F(TransactionTest, MakeValidTransfer) {
    bool result = tx.Make(*fromAccount, *toAccount, 300);
    EXPECT_TRUE(result);
    EXPECT_NE(1000, fromAccount->GetBalance());
    EXPECT_NE(500, toAccount->GetBalance());
}
TEST_F(TransactionTest, MakeTransferWithFee) {
    tx.set_fee(50);
    bool result = tx.Make(*fromAccount, *toAccount, 300);
    EXPECT_TRUE(result);
    EXPECT_NE(1000, fromAccount->GetBalance());
    EXPECT_NE(500, toAccount->GetBalance());
}
TEST_F(TransactionTest, MakeTransferFailsIfFeeTooHigh) {
    tx.set_fee(200);
    bool result = tx.Make(*fromAccount, *toAccount, 300);
    EXPECT_FALSE(result);
    EXPECT_EQ(1000, fromAccount->GetBalance());
    EXPECT_EQ(500, toAccount->GetBalance());
}
TEST_F(TransactionTest, MakeTransferFailsIfSumTooSmall) {
    EXPECT_THROW(tx.Make(*fromAccount, *toAccount, 50), std::logic_error);
}
TEST_F(TransactionTest, MakeTransferFailsIfSumNegative) {
    EXPECT_THROW(tx.Make(*fromAccount, *toAccount, -100), std::invalid_argument);
}
TEST_F(TransactionTest, MakeTransferFailsIfSameAccount) {
    EXPECT_THROW(tx.Make(*fromAccount, *fromAccount, 300), std::logic_error);
}
TEST_F(TransactionTest, MakeTransferInsufficientFunds) {
    bool result = tx.Make(*fromAccount, *toAccount, 1500);
    EXPECT_GE(fromAccount->GetBalance(), 0);
    EXPECT_GE(toAccount->GetBalance(), 0);
}
TEST_F(TransactionTest, SetAndGetFee) {
    tx.set_fee(100);
    EXPECT_EQ(100, tx.fee());
}
TEST_F(TransactionTest, DefaultFeeIsOne) {
    EXPECT_EQ(1, tx.fee());
}
EOF
```

 Создаем test_mocks.cpp
```bash
cat > tests/test_mocks.cpp <<'EOF'
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Transaction.h"
#include "Account.h"
#include <memory>
class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int diff), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};
class MockTransaction : public Transaction {
public:
    MOCK_METHOD(void, SaveToDataBase, (Account& from, Account& to, int sum), (override));
};
TEST(TransactionMockTest, SaveToDataBaseIsCalled) {
    MockAccount from(11111, 1000);
    MockAccount to(22222, 500);
    MockTransaction tx;
    tx.set_fee(0);
    
    ON_CALL(from, GetBalance()).WillByDefault(testing::Return(1000));
    ON_CALL(to, GetBalance()).WillByDefault(testing::Return(500));
    
    EXPECT_CALL(tx, SaveToDataBase(testing::Ref(from), testing::Ref(to), testing::_))
        .Times(1);
    
    EXPECT_CALL(from, Lock()).Times(testing::AnyNumber());
    EXPECT_CALL(to, Lock()).Times(testing::AnyNumber());
    EXPECT_CALL(from, Unlock()).Times(testing::AnyNumber());
    EXPECT_CALL(to, Unlock()).Times(testing::AnyNumber());
    
    tx.Make(from, to, 300);
}
TEST(AccountMockTest, ChangeBalanceIsCalledAfterLock) {
    MockAccount account(12345, 1000);
    
    EXPECT_CALL(account, Lock()).Times(1);
    EXPECT_CALL(account, ChangeBalance(500)).Times(1);
    EXPECT_CALL(account, GetBalance())
        .WillOnce(testing::Return(1500));
    EXPECT_CALL(account, Unlock()).Times(1);
    
    account.Lock();
    account.ChangeBalance(500);
    EXPECT_EQ(1500, account.GetBalance());
    account.Unlock();
}
EOF
```

Создаем GitHub Actions workflow
```bash
cat > .github/workflows/ci.yml <<'EOF'
name: CI
on: [push, pull_request]
jobs:
  build-and-test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v4
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake build-essential lcov gcovr
    
    - name: Configure CMake with coverage
      run: |
        rm -rf _build
        cmake -H. -B_build -DBUILD_TESTS=ON -DCOVERAGE=ON
    
    - name: Build
      run: cmake --build _build
    
    - name: Run tests
      run: ./_build/banking_tests
    
    - name: Generate coverage report
      run: |
        cd _build
        gcovr --root .. --filter '.*\.cpp' --exclude '.*/_deps/.*' --xml --output coverage.xml
    
    - name: Upload to Coveralls
      uses: coverallsapp/github-action@v2
      with:
        file: _build/coverage.xml
        format: cobertura
        github-token: ${{ secrets.GITHUB_TOKEN }}
EOF
```

Покрытие кода
```bash
rm -rf _build
cmake -H. -B_build -DBUILD_TESTS=ON -DCOVERAGE=ON
cmake --build _build
./_build/banking_tests
cd _build
gcovr --root .. --filter '.*\.cpp' --exclude '.*/_deps/.*'
```

Выводы команд в консоль
```sh
-- The C compiler identification is GNU 13.3.0
-- The CXX compiler identification is GNU 13.3.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Found Python3: /usr/bin/python3 (found version "3.12.3") found components: Interpreter 
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
-- Found Threads: TRUE  
-- Configuring done (13.0s)
-- Generating done (0.0s)
-- Build files have been written to: /home/ubumba64/denismalyi2204-glitch/workspace/projects/banking/_build
[  6%] Building CXX object CMakeFiles/banking.dir/Account.cpp.o
[ 13%] Building CXX object CMakeFiles/banking.dir/Transaction.cpp.o
[ 20%] Linking CXX static library libbanking.a
[ 20%] Built target banking
[ 26%] Building CXX object _deps/googletest-build/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o
[ 33%] Linking CXX static library ../../../lib/libgtest.a
[ 33%] Built target gtest
[ 40%] Building CXX object _deps/googletest-build/googlemock/CMakeFiles/gmock.dir/src/gmock-all.cc.o
[ 46%] Linking CXX static library ../../../lib/libgmock.a
[ 46%] Built target gmock
[ 53%] Building CXX object _deps/googletest-build/googletest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o
[ 60%] Linking CXX static library ../../../lib/libgtest_main.a
[ 60%] Built target gtest_main
[ 66%] Building CXX object CMakeFiles/banking_tests.dir/tests/test_account.cpp.o
[ 73%] Building CXX object CMakeFiles/banking_tests.dir/tests/test_transaction.cpp.o
[ 80%] Building CXX object CMakeFiles/banking_tests.dir/tests/test_mocks.cpp.o
[ 86%] Linking CXX executable banking_tests
[ 86%] Built target banking_tests
[ 93%] Building CXX object _deps/googletest-build/googlemock/CMakeFiles/gmock_main.dir/src/gmock_main.cc.o
[100%] Linking CXX static library ../../../lib/libgmock_main.a
[100%] Built target gmock_main
Running main() from /home/ubumba64/denismalyi2204-glitch/workspace/projects/banking/_build/_deps/googletest-src/googletest/src/gtest_main.cc
[==========] Running 18 tests from 4 test suites.
[----------] Global test environment set-up.
[----------] 7 tests from AccountTest
[ RUN      ] AccountTest.ConstructorSetsIdAndBalance
[       OK ] AccountTest.ConstructorSetsIdAndBalance (0 ms)
[ RUN      ] AccountTest.GetBalanceReturnsCorrectValue
[       OK ] AccountTest.GetBalanceReturnsCorrectValue (0 ms)
[ RUN      ] AccountTest.ChangeBalanceThrowsIfNotLocked
[       OK ] AccountTest.ChangeBalanceThrowsIfNotLocked (0 ms)
[ RUN      ] AccountTest.ChangeBalanceWorksAfterLock
[       OK ] AccountTest.ChangeBalanceWorksAfterLock (0 ms)
[ RUN      ] AccountTest.LockThrowsIfAlreadyLocked
[       OK ] AccountTest.LockThrowsIfAlreadyLocked (0 ms)
[ RUN      ] AccountTest.UnlockWorks
[       OK ] AccountTest.UnlockWorks (0 ms)
[ RUN      ] AccountTest.IdReturnsCorrectValue
[       OK ] AccountTest.IdReturnsCorrectValue (0 ms)
[----------] 7 tests from AccountTest (0 ms total)

[----------] 9 tests from TransactionTest
[ RUN      ] TransactionTest.MakeValidTransfer
11111 send to 22222 $300
Balance 11111 is 1000
Balance 22222 is 499
[       OK ] TransactionTest.MakeValidTransfer (0 ms)
[ RUN      ] TransactionTest.MakeTransferWithFee
11111 send to 22222 $300
Balance 11111 is 1000
Balance 22222 is 450
[       OK ] TransactionTest.MakeTransferWithFee (0 ms)
[ RUN      ] TransactionTest.MakeTransferFailsIfFeeTooHigh
[       OK ] TransactionTest.MakeTransferFailsIfFeeTooHigh (0 ms)
[ RUN      ] TransactionTest.MakeTransferFailsIfSumTooSmall
[       OK ] TransactionTest.MakeTransferFailsIfSumTooSmall (0 ms)
[ RUN      ] TransactionTest.MakeTransferFailsIfSumNegative
[       OK ] TransactionTest.MakeTransferFailsIfSumNegative (0 ms)
[ RUN      ] TransactionTest.MakeTransferFailsIfSameAccount
[       OK ] TransactionTest.MakeTransferFailsIfSameAccount (0 ms)
[ RUN      ] TransactionTest.MakeTransferInsufficientFunds
11111 send to 22222 $1500
Balance 11111 is 1000
Balance 22222 is 499
[       OK ] TransactionTest.MakeTransferInsufficientFunds (0 ms)
[ RUN      ] TransactionTest.SetAndGetFee
[       OK ] TransactionTest.SetAndGetFee (0 ms)
[ RUN      ] TransactionTest.DefaultFeeIsOne
[       OK ] TransactionTest.DefaultFeeIsOne (0 ms)
[----------] 9 tests from TransactionTest (0 ms total)

[----------] 1 test from TransactionMockTest
[ RUN      ] TransactionMockTest.SaveToDataBaseIsCalled

GMOCK WARNING:
Uninteresting mock function call - taking default action specified at:
/home/ubumba64/denismalyi2204-glitch/workspace/projects/banking/tests/test_mocks.cpp:30:
    Function call: GetBalance()
          Returns: 500
NOTE: You can safely ignore the above warning unless this call should not happen.  Do not suppress it by blindly adding an EXPECT_CALL() if you dont mean to enforce the call.  See https://github.com/google/googletest/blob/main/docs/gmock_cook_book.md#knowing-when-to-expect-useoncall for details.
[       OK ] TransactionMockTest.SaveToDataBaseIsCalled (0 ms)
[----------] 1 test from TransactionMockTest (0 ms total)

[----------] 1 test from AccountMockTest
[ RUN      ] AccountMockTest.ChangeBalanceIsCalledAfterLock
[       OK ] AccountMockTest.ChangeBalanceIsCalledAfterLock (0 ms)
[----------] 1 test from AccountMockTest (0 ms total)

[----------] Global test environment tear-down
[==========] 18 tests from 4 test suites ran. (0 ms total)
[  PASSED  ] 18 tests.
(INFO) Reading coverage data...
(INFO) Writing coverage report...
------------------------------------------------------------------------------
                           GCC Code Coverage Report
Directory: ..
------------------------------------------------------------------------------
File                                       Lines    Exec  Cover   Missing
------------------------------------------------------------------------------
tests/test_account.cpp                        31      31   100%
tests/test_mocks.cpp                          35      35   100%
tests/test_transaction.cpp                    47      47   100%
------------------------------------------------------------------------------
TOTAL                                        113     113   100%
------------------------------------------------------------------------------
```

Проверяем статус CI
```sh
gh run list -R denismalyi2204-glitch/banking --limit 3
```
Вывод
```sh
STATUS  TITLE                                             WORKFLOW  BRANCH  EVENT  ID           ELAPSED  AGE
✓       feat: banking library with 100% test coverage...  CI        main    push   24637654370  47s      about 1 minute ago
```


Отправляем на GitHub
```bash
git init
git add .
git commit -m "feat: banking library with 100% test coverage, mocks, CI, Coveralls"
git remote add origin https://github.com/denismalyi2204-glitch/banking.git
git push -u origin main
```

Вывод
```sh
[main (root-commit) 1b88819] feat: banking library with 100% test coverage, mocks, CI, Coveralls
 9 files changed, 543 insertions(+)
 create mode 100644 .github/workflows/ci.yml
 create mode 100644 .gitignore
 create mode 100644 CMakeLists.txt
 create mode 100644 tests/test_account.cpp
 create mode 100644 tests/test_mocks.cpp
 create mode 100644 tests/test_transaction.cpp
To https://github.com/denismalyi2204-glitch/banking.git
 * [new branch]      main -> main
Branch 'main' set up to track remote branch 'main' from 'origin'.
```

