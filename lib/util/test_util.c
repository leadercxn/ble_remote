#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CuTest.h"

#include "util.h"


void TestUtilHexStrCheck(CuTest* tc)
{
    bool rc = false;

    rc = hexstr_check("11223344");
    CuAssert(tc, "HexStrCheck", true == rc);
    
    rc = hexstr_check("11223344aabbccdd");
    CuAssert(tc, "HexStrCheck", true == rc);
    
    rc = hexstr_check("112233445");
    CuAssert(tc, "HexStrCheck", false == rc);
    
    rc = hexstr_check("11223344aabbccdp");
    CuAssert(tc, "HexStrCheck", false == rc);
    
    rc = hexstr_check("1122334iaabbccdd");
    CuAssert(tc, "HexStrCheck", false == rc);
    
    rc = hexstr_check("y11223344aabbccdd");
    CuAssert(tc, "HexStrCheck", false == rc);
}

void TestUtilIntegerCheck(CuTest* tc)
{
    bool rc = false;

    rc = integer_check("11223344");
    CuAssert(tc, "IntegerCheck", true == rc);
    
    rc = integer_check("+12345");
    CuAssert(tc, "IntegerCheck", true == rc);
    
    rc = integer_check("-543121");
    CuAssert(tc, "IntegerCheck", true == rc);
    
    rc = integer_check("123a");
    CuAssert(tc, "IntegerCheck", false == rc);
    
    rc = integer_check("a1234");
    CuAssert(tc, "IntegerCheck", false == rc);
    
    rc = integer_check("12t45");
    CuAssert(tc, "IntegerCheck", false == rc);
    
    rc = integer_check("543121+");
    CuAssert(tc, "IntegerCheck", false == rc);
    
    rc = integer_check("543121-");
    CuAssert(tc, "IntegerCheck", false == rc);
}

CuSuite* TestUtilCuGetSuite(void)
{
	CuSuite* suite = CuSuiteNew();
    
    SUITE_ADD_TEST(suite, TestUtilHexStrCheck);
    SUITE_ADD_TEST(suite, TestUtilIntegerCheck);

	return suite;
}

