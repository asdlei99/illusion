/*!
* @copyright  2004-2014  Apache License, Version 2.0 FULLSAIL
* @filename   zce_protobuf_reflect.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       Tuesday, November 25, 2014
* @brief
*
*
* @details
*
*
*
* @note
* 《关于郑州的记忆》 李志
* 关于郑州我知道的不多，为了爱情曾经去过那里
* 多少次在火车上路过这城市，一个人悄悄地想起她
* 她说她喜欢郑州冬天的阳光，巷子里飘满煤炉的味道
* 雾气穿过她年轻的脖子，直到今天都没有散去
* 关于郑州我想的全是你，想来想去都是忏悔和委屈
* 关于郑州我爱的全是你，爱来爱去不明白爱的意义
* 关于郑州只是偶尔想起，她的味道都在回忆里
* 每次和朋友说起过去的旅行，我不敢说我曾去过那里
* 关于郑州我想的全是你，想来生活无非是痛苦和美丽
* 关于郑州我爱的全是你，爱到最后我们都无路可去
* 似是而非或是世事可畏，有情有义又是有米无炊
* 时间改变了很多又什么都没有
* 让我再次拥抱你，郑州
*/

#ifndef _ILLUSION_PROTOBUF_REFLECT_H_
#define _ILLUSION_PROTOBUF_REFLECT_H_



//======================================================================================

/*!
* @brief
*
* @note
*/
class Protobuf_Reflect_AUX
{
    //
public:


    /*!
    * @brief      根据fullname，也就是 phone_book.number 设置一个Message的field
    * @return     int
    * @param      msg          对应要操作的msg
    * @param      full_name    字段的全名，如果是多个结构字段可以用.链接
    * @param      set_data     设置的数据
    * @param      repeated_add 是否是repeated 字段，是进行添加操作
    */
    static int set_field(google::protobuf::Message *msg,
                         const std::string &full_name,
                         const std::string &set_data,
                         bool repeated_add);

    //!根据fullname,得到某个字段的描述信息
    static int get_fielddesc(google::protobuf::Message *msg,
                             const std::string &full_name,
                             bool message_add,
                             google::protobuf::Message *&field_msg,
                             const google::protobuf::FieldDescriptor *&field_desc);

    //!设置一个Message的field
    static int set_fielddata(google::protobuf::Message *msg,
                             const google::protobuf::FieldDescriptor *field,
                             const std::string &set_data);

    static int set_fielddata(google::protobuf::Message *msg,
                             const google::protobuf::FieldDescriptor *field,
                             const QString &set_data);

    //!定位一个子结构
    static int locate_sub_msg(google::protobuf::Message *msg,
                              const std::string &submsg_field_name,
                              bool message_add,
                              google::protobuf::Message *&sub_msg);


    //!定位一个子结构
    static int locate_msgfield(google::protobuf::Message *msg,
                               const google::protobuf::FieldDescriptor *msg_field,
                               google::protobuf::Message *&sub_msg,
                               bool message_add);

    //!打印输出一个Message的信息到ostream里面，
    static void protobuf_output(const google::protobuf::Message *msg,
                                std::ostream *out);

    //!将message里面所有的字段设置为默认值，
    //!注意这儿不直接用Clear函数的原因是，我在读取配置的过程已经把Message结构构造好了
    //!对应的Field的指针也保存了，如果Clear这一切都失效了（对repeated里面新增的Mesage）。
    static void message_set_default(google::protobuf::Message *msg);


    //!
    static bool string_to_bool(const std::string &str);

    //!
    static bool qstring_to_bool(const QString &str);

    static void string_split(const std::string &source_str,
                             const std::string &separator,
                             std::vector<std::string> &v);




};



#endif //# _ILLUSION_PROTOBUF_REFLECT_H_


