#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

#define HOST "127.0.0.1"
#define USER "tom"
#define PASSWD "tom"
#define DB "hr"
#define PORT 3306

/*
 * 使用预处理语句插入部门信息。
 * name 和 city 通过占位符绑定，不会被解释成 SQL 代码，因而可以防止 SQL 注入。
 * 返回 0 表示成功，返回非 0 表示失败。
 */
static int insert_dept(MYSQL *conn, char *name, char *city)
{
    /* ? 是参数占位符，不能给表名或列名作占位符。 */
    const char *sql = "INSERT INTO dept(name, city) VALUES(?, ?)";

    /* 创建预处理语句对象；使用完毕后必须调用 mysql_stmt_close()。 */
    MYSQL_STMT *stmt = mysql_stmt_init(conn);

    /* MYSQL_BIND 描述每个参数的类型、地址和实际长度，使用前必须清零。 */
    MYSQL_BIND params[2] = {0};
    unsigned long name_len = (unsigned long)strlen(name);
    unsigned long city_len = (unsigned long)strlen(city);

    if (!stmt)
    {
        fprintf(stderr, "创建预处理语句失败：%s\n", mysql_error(conn));
        return 1;
    }

    /*
     * 让数据库解析 SQL 模板。此时只确定 SQL 的结构，
     * name 和 city 的具体内容将在后面单独绑定。
     */
    if (mysql_stmt_prepare(stmt, sql, (unsigned long)strlen(sql)))
    {
        fprintf(stderr, "预处理失败：%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return 1;
    }

    /* 绑定第一个 ?：部门名称。 */
    params[0].buffer_type = MYSQL_TYPE_STRING;
    params[0].buffer = name;
    params[0].buffer_length = name_len;
    params[0].length = &name_len;

    /* 绑定第二个 ?：所在城市。 */
    params[1].buffer_type = MYSQL_TYPE_STRING;
    params[1].buffer = city;
    params[1].buffer_length = city_len;
    params[1].length = &city_len;

    /* 按占位符出现的顺序，把 params 中的参数绑定到 SQL。 */
    if (mysql_stmt_bind_param(stmt, params))
    {
        fprintf(stderr, "绑定参数失败：%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return 1;
    }

    /* 执行已经预处理并完成参数绑定的 INSERT 语句。 */
    if (mysql_stmt_execute(stmt))
    {
        fprintf(stderr, "执行插入失败：%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return 1;
    }

    printf("插入成功，影响行数：%llu\n",
           (unsigned long long)mysql_stmt_affected_rows(stmt));

    /* 释放预处理语句对象。 */
    mysql_stmt_close(stmt);
    return 0;
}

int main(void)
{
    /* 初始化 MYSQL 连接句柄。 */
    MYSQL *conn = mysql_init(NULL);
    if (!conn)
    {
        fprintf(stderr, "初始化数据库连接失败\n");
        return EXIT_FAILURE;
    }

    /* 连接 MySQL 服务器并选择 hr 数据库。 */
    if (!mysql_real_connect(conn, HOST, USER, PASSWD, DB, PORT, NULL, 0))
    {
        fprintf(stderr, "连接失败：%u, %s\n",
                mysql_errno(conn), mysql_error(conn));
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    /*
     * 需要写入数据库的数据。即使数据包含引号或 SQL 关键字，
     * 预处理语句也只会把它当作普通字段值处理。
     */
    char dept[] = "123";
    char city[] = "456";
    if (insert_dept(conn, dept, city))
    {
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    /* 此查询是固定 SQL，不包含外部输入，因此不存在字符串拼接注入问题。 */
    const char *sql = "SELECT id, name, city FROM dept";
    if (mysql_query(conn, sql))
    {
        fprintf(stderr, "执行查询失败：%s\n", mysql_error(conn));
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    /* 把服务器返回的全部查询结果保存到客户端内存。 */
    MYSQL_RES *res = mysql_store_result(conn);
    if (!res)
    {
        fprintf(stderr, "获取结果集失败：%s\n", mysql_error(conn));
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    unsigned int cols = mysql_num_fields(res);
    printf("行：%llu，列：%u\n",
           (unsigned long long)mysql_num_rows(res), cols);

    /* 获取并输出所有列的名称。 */
    MYSQL_FIELD *fields = mysql_fetch_fields(res);
    for (unsigned int i = 0; i < cols; i++)
        printf("%s\t", fields[i].name);
    printf("\n");

    /* 逐行读取结果；数据库中的 SQL NULL 单独显示为字符串 "NULL"。 */
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        for (unsigned int i = 0; i < cols; i++)
            printf("%s\t", row[i] ? row[i] : "NULL");
        printf("\n");
    }

    /* 先释放结果集，再关闭数据库连接。 */
    mysql_free_result(res);
    mysql_close(conn);
    return EXIT_SUCCESS;
}
