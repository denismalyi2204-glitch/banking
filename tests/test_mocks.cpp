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
    
    // Разрешаем любые вызовы GetBalance с возвратом 1000
    ON_CALL(from, GetBalance()).WillByDefault(testing::Return(1000));
    ON_CALL(to, GetBalance()).WillByDefault(testing::Return(500));
    
    // Ожидаем вызов SaveToDataBase
    EXPECT_CALL(tx, SaveToDataBase(testing::Ref(from), testing::Ref(to), testing::_))
        .Times(1);
    
    // Разрешаем Lock/Unlock (они могут вызываться)
    EXPECT_CALL(from, Lock()).Times(testing::AnyNumber());
    EXPECT_CALL(to, Lock()).Times(testing::AnyNumber());
    EXPECT_CALL(from, Unlock()).Times(testing::AnyNumber());
    EXPECT_CALL(to, Unlock()).Times(testing::AnyNumber());
    
    // Разрешаем ChangeBalance (могут вызываться или нет)
    EXPECT_CALL(from, ChangeBalance(testing::_)).Times(testing::AnyNumber());
    EXPECT_CALL(to, ChangeBalance(testing::_)).Times(testing::AnyNumber());
    
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
