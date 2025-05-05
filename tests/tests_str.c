#include <stdbool.h>

#include "tests.h"
#include "tests_str.h"

int test_str_to()
{
    char buf[13];
#ifndef FIXMATH_NO_ROUNDING // Test cases with ROUNDED results

    fix16_to_str(fix16_from_dbl(1234.5678), buf, 4);
    ASSERT_EQ_STR(buf, "1234.5678");

    fix16_to_str(fix16_from_dbl(-1234.5678), buf, 4);
    ASSERT_EQ_STR(buf, "-1234.5678");

    fix16_to_str(0, buf, 0);
    ASSERT_EQ_STR(buf, "0");

    fix16_to_str(0, buf, 3);
    ASSERT_EQ_STR(buf, "0.000");

    fix16_to_str(fix16_from_dbl(0.9), buf, 0);
    ASSERT_EQ_STR(buf, "1");

    fix16_to_str(1, buf, 5);
    ASSERT_EQ_STR(buf, "0.00002");

    fix16_to_str(-1, buf, 5);
    ASSERT_EQ_STR(buf, "-0.00002");

    fix16_to_str(65535, buf, 5);
    ASSERT_EQ_STR(buf, "0.99998");

    fix16_to_str(65535, buf, 4);
    ASSERT_EQ_STR(buf, "1.0000");

    fix16_to_str(fix16_maximum, buf, 5);
    ASSERT_EQ_STR(buf, "32767.99998");

    fix16_to_str(fix16_minimum, buf, 5);
    ASSERT_EQ_STR(buf, "-32768.00000");

#else // Test cases with TRUNCATED results

    fix16_to_str(fix16_from_dbl(1234.5678), buf, 4);
    ASSERT_EQ_STR(buf, "1234.5677");

    fix16_to_str(fix16_from_dbl(-1234.5678), buf, 4);
    ASSERT_EQ_STR(buf, "-1234.5677");

    fix16_to_str(0, buf, 0);
    ASSERT_EQ_STR(buf, "0");

    fix16_to_str(0, buf, 3);
    ASSERT_EQ_STR(buf, "0.000");

    fix16_to_str(fix16_from_dbl(0.9), buf, 0);
    ASSERT_EQ_STR(buf, "0");

    fix16_to_str(1, buf, 5);
    ASSERT_EQ_STR(buf, "0.00001");

    fix16_to_str(-1, buf, 5);
    ASSERT_EQ_STR(buf, "-0.00001");

    fix16_to_str(65535, buf, 5);
    ASSERT_EQ_STR(buf, "0.99998");

    fix16_to_str(65535, buf, 4);
    ASSERT_EQ_STR(buf, "0.9999");

    fix16_to_str(fix16_maximum, buf, 5);
    ASSERT_EQ_STR(buf, "32767.99998");

    fix16_to_str(fix16_minimum, buf, 5);
    ASSERT_EQ_STR(buf, "-32768.00000");

#endif

    return 0;
}

int test_str_from()
{
#ifndef FIXMATH_NO_ROUNDING // Test cases with ROUNDED results

    ASSERT_EQ_INT(fix16_from_str("1234.5678"), fix16_from_dbl(1234.5678));
    ASSERT_EQ_INT(fix16_from_str("-1234.5678"), fix16_from_dbl(-1234.5678));
    ASSERT_EQ_INT(fix16_from_str("   +1234,56780   "),
                  fix16_from_dbl(1234.5678));

    ASSERT_EQ_INT(fix16_from_str("0"), 0);
    ASSERT_EQ_INT(fix16_from_str("1"), fix16_one);
    ASSERT_EQ_INT(fix16_from_str("1.0"), fix16_one);
    ASSERT_EQ_INT(fix16_from_str("1.0000000000"), fix16_one);

    ASSERT_EQ_INT(fix16_from_str("0.00002"), 1);
    ASSERT_EQ_INT(fix16_from_str("0.99998"), 65535);

    ASSERT_EQ_INT(fix16_from_str("32767.99998"), fix16_maximum);
    ASSERT_EQ_INT(fix16_from_str("-32768.00000"), fix16_minimum);

#else // Test cases with TRUNCATED results

    ASSERT_EQ_INT(fix16_from_str("1234.5678"), fix16_from_dbl(1234.5678));
    ASSERT_EQ_INT(fix16_from_str("-1234.5678"), fix16_from_dbl(-1234.5678));
    ASSERT_EQ_INT(fix16_from_str("   +1234,56780   "),
                  fix16_from_dbl(1234.5678));

    ASSERT_EQ_INT(fix16_from_str("0"), 0);
    ASSERT_EQ_INT(fix16_from_str("1"), fix16_one);
    ASSERT_EQ_INT(fix16_from_str("1.0"), fix16_one);
    ASSERT_EQ_INT(fix16_from_str("1.0000000000"), fix16_one);

    ASSERT_EQ_INT(fix16_from_str("0.00002"), 1);
    ASSERT_EQ_INT(fix16_from_str("0.99998"), 65534);

    ASSERT_EQ_INT(fix16_from_str("32767.99998"), fix16_maximum - 1);
    ASSERT_EQ_INT(fix16_from_str("-32768.00000"), fix16_minimum);

#endif

    return 0;
}

int test_str_extended()
{

    fix16_t value = (fix16_t)0x80000000U;

    char    testbuf[13];
    char    goodbuf[13];

    while (value < fix16_maximum)
    {
        double fvalue = fix16_to_dbl(value);

        /* Turns out we have to jump through some hoops to round
        doubles perfectly for printing:
        http://stackoverflow.com/questions/994764/rounding-doubles-5-sprintf
        */

        double rvalue  = round(fvalue * 100000.) / 100000.;

        double exp_val = rvalue;
        snprintf(goodbuf, 13, "%0.5f", exp_val);
        fix16_to_str(value, testbuf, 5);

#ifndef FIXMATH_NO_ROUNDING

        if (strcmp(goodbuf, testbuf) != 0)
        {
            printf("Value %f/(fix16_t)%d gave %s, should be %s\n", rvalue,
                   value, testbuf, goodbuf);
            return 1;
        }

#else
        bool negative = (value < 0);
        if (strcmp(goodbuf, testbuf) != 0)
        {
            double d_sub = negative ? -0.0000050001 : 0.0000050001;

            exp_val -= d_sub;
            snprintf(goodbuf, 13, "%0.5f", exp_val);
            fix16_to_str(value, testbuf, 5);

            if (strcmp(goodbuf, testbuf) != 0)
            {
                printf("Value %f/(fix16_t)%d gave %s, should be %s\n", rvalue,
                       value, testbuf, goodbuf);
                return 1;
            }
        }

#endif

        fix16_t roundtrip = fix16_from_str(testbuf);

#ifndef FIXMATH_NO_ROUNDING
        if (roundtrip != value)
        {
            printf("%f\n", rvalue);

            printf("Roundtrip failed: %f -> (fix16_t)%d -> %s -> (fix16_t)%d\n",
                   fvalue, value, testbuf, roundtrip);
            return 1;
        }
#else

        fix16_t sub = negative ? -1 : 1;

        bool    close_enough =
            ((roundtrip == value) || (roundtrip == value - sub));
        if (!close_enough)
        {
            printf("Roundtrip failed: %f -> (fix16_t)%d -> %s -> (fix16_t)%d\n",
                   rvalue, value - 0, testbuf, roundtrip);
            return 1;
        }
#endif

        value += 0x00010001;
    }

    return 0;
}

int test_str()
{
    TEST(test_str_to());
    TEST(test_str_from());
    TEST(test_str_extended());
    return 0;
}
