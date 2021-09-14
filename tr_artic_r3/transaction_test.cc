

#include "internal/transaction.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "internal/mock_io.h"
#include "pw_status/status.h"

using ::testing::Return;

namespace tr::artic::internal {
TEST(TransactionContext, BeginEnd) {
  MockSpi spi;

  MockGpi interrupt1;
  EXPECT_CALL(interrupt1, IsHigh())
      .WillOnce(Return(false))
      .WillOnce(Return(true));

  MockGpi interrupt2;
  EXPECT_CALL(interrupt2, IsHigh()).WillRepeatedly(Return(false));

  TransactionContext transaction(spi, interrupt1, interrupt2);

  pw::Status status = transaction.Begin();
  EXPECT_EQ(status, pw::OkStatus());

  status = transaction.End();
  EXPECT_EQ(status, pw::OkStatus());
}

}  // namespace tr::artic::internal