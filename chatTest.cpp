//#include <iostream>
//#include <string>
//
//#include <jdbc/mysql_driver.h>
//#include <jdbc/mysql_connection.h>
//#include <jdbc/cppconn/statement.h>
//
//const std::string address{"tcp://127.0.0.1:3306"};
//const std::string username{ "root" };
//const std::string password{ "92110876540a" };
//const std::string database{ "ejercicio" };
//
//int main() {
//    sql::mysql::MySQL_Driver* driver;
//    sql::Connection* con;
//
//    try {
//        driver = sql::mysql::get_mysql_driver_instance();
//        con = driver->connect(address, username, password);
//        con->setSchema(database); // Replace with your actual database name
//
//        sql::Statement* stmt = con->createStatement();
//        stmt->execute("CREATE TABLE example_table (id INT PRIMARY KEY, name VARCHAR(50))");
//
//        delete stmt;
//        delete con;
//    }
//    catch (sql::SQLException& e) {
//        std::cout << "SQLException: " << e.what() << std::endl;
//    }
//
//    return 0;
//}
