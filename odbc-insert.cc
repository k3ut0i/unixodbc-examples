
#include<sql.h>
#include<sqlext.h>
#include<exception>
#include<iostream>
#include<fstream>

void show_error() {
  printf("SQL fetch Error");
}
/**
 * @brief error handling for SQL* functions
 */
void check_error(SQLRETURN ret_val, std::string act_str)
{
  switch (ret_val)
    {
    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO:
      break;
    case SQL_NO_DATA:
      std::cerr << "end of data\n";
      break;
    default:
      throw std::runtime_error(act_str);
    }
}

int main() {
  SQLHENV henv;
  SQLHDBC hdbc;
  SQLHSTMT hstmt = 0;
  SQLRETURN retcode;
  SQLCHAR outstr[1024];
  SQLSMALLINT outstrlen;

  try{
    const char* file_name = "./odbc-insert.cc";  
    std::ifstream image_file(file_name, std::ios::binary);
    image_file.seekg(0, std::ios::end);
    size_t file_size = image_file.tellg();
    image_file.seekg(0, std::ios::beg);
    char* file_buf = new char[file_size];
 
    image_file.read(file_buf, file_size);
    image_file.close();
 

    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    check_error(retcode, "Allocate Environment Handle");

    retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);
    check_error(retcode, "Set ODBC version enviroment attribute");

    retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
    check_error(retcode, "Allocate connection Handle");

    SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
    check_error(retcode, "set login timeout attribute");

    retcode = SQLDriverConnect(hdbc, NULL, (SQLCHAR*)"DSN=psql-images;", SQL_NTS, outstr, sizeof(outstr), &outstrlen, SQL_DRIVER_COMPLETE);
    check_error(retcode, "connect to the data source");

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    check_error(retcode, "allocate a statement handle");

    //char* strstmt = new char[file_size + 100];
    //sprintf(strstmt, "insert into images values('%s', 'key')", file_buf);
    // printf("%s", file_buf);
    //retcode = SQLExecDirect (hstmt, (SQLCHAR *) strstmt, SQL_NTS);
    //retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, file_size, 0, file_buf, file_size, 0);
    int val1 = 1;
    const char* name = "myparamname";
    SQLCHAR* insert_statement = (SQLCHAR*)"insert into mytable values(?,?)";
    SQLINTEGER pathstrlen = SQL_NTS;
    retcode = SQLPrepare(hstmt, insert_statement, SQL_NTS);
    check_error(retcode, "Statement Preparation Error");
    retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &val1, sizeof(int), NULL);
    check_error(retcode, "BindParameter Error");
    retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, sizeof(name), 0, (SQLCHAR*)name, 0, NULL);
    check_error(retcode, "BindParameter Error");
    retcode = SQLExecute(hstmt);
    check_error(retcode, "Statement Execution Error");
    
    SQLCancel(hstmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

    SQLDisconnect(hdbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, henv);
  }
  catch(std::exception &e)
    {
      std::cerr << e.what() << std::endl;
      return 1;
    }
  return 0;
}
