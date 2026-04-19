[![CI](https://github.com/denismalyi2204-glitch/banking/actions/workflows/ci.yml/badge.svg)](https://github.com/denismalyi2204-glitch/banking/actions/workflows/ci.yml)
[![Coverage Status](https://coveralls.io/repos/github/denismalyi2204-glitch/banking/badge.svg)](https://coveralls.io/github/denismalyi2204-glitch/banking)

# Banking Library

Библиотека для банковских операций с классами:
- `Account` - банковский счет
- `Transaction` - транзакция между счетами

## Сборка и тестирование

```bash
cmake -H. -B_build -DBUILD_TESTS=ON
cmake --build _build
./_build/banking_tests
