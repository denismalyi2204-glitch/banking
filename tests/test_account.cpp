#include <gtest/gtest.h>
#include "Account.h"
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
