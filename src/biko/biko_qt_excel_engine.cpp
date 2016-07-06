#include "biko_predefine.h"
#include "biko_qt_excel_engine.h"


BikoQtExcelEngine::BikoQtExcelEngine()
{
    excel_instance_     = NULL;
    work_books_ = NULL;
    active_book_  = NULL;
    active_sheet_ = NULL;

    xls_file_     = "";

    curr_sheet_ = 1;
    row_count_ = 0;
    column_count_ = 0;
    start_row_    = 0;
    start_column_ = 0;

    is_open_     = false;
    is_valid_    = false;
    is_a_newfile_ = false;
    is_save_already_ = false;

}

BikoQtExcelEngine::~BikoQtExcelEngine()
{
    if ( is_open_ )
    {
        //����ǰ���ȱ������ݣ�Ȼ��ر�workbook
        close();
    }
    finalize();
}


//��ʼ��EXCEL OLE���󣬴�EXCEL ���̣�
bool BikoQtExcelEngine::initialize(bool visible)
{

    HRESULT r = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (r != S_OK && r != S_FALSE)
    {
        qDebug("Qt: Could not initialize OLE (error %x)", (unsigned int)r);
        return false;
    }
    is_visible_ = visible;
    //
    if (NULL == excel_instance_)
    {
        excel_instance_ = new QAxObject("Excel.Application");
        if (excel_instance_->isNull())
        {
            is_valid_ = false;
            is_open_ = false;
            return is_open_;
        }
        else
        {
            is_valid_ = true;
        }

        excel_instance_->dynamicCall("SetVisible(bool)", is_visible_);
    }
    return true;
}


//
void BikoQtExcelEngine::finalize()
{
    if (excel_instance_ )
    {

        excel_instance_->dynamicCall("Quit()");

        delete excel_instance_;
        excel_instance_ = NULL;

        is_open_ = false;
        is_valid_ = false;
        is_a_newfile_ = false;
        is_save_already_ = true;
    }

    ::CoUninitialize();
}


//��EXCEL�ļ�
bool BikoQtExcelEngine::open(const QString &xls_file, int  sheet_index)
{
    xls_file_ = xls_file;
    curr_sheet_ = sheet_index;

    if (is_open_)
    {
        close();
    }

    curr_sheet_ = sheet_index;

    if (!is_valid_)
    {
        is_open_ = false;
        return is_open_;
    }

    //���ָ����ļ������ڣ�����Ҫ�½�һ��
    QFileInfo fi(xls_file_);
    if (!fi.exists())
    {
        is_a_newfile_ = true;
    }
    else
    {
        is_a_newfile_ = false;
    }

    work_books_ = excel_instance_->querySubObject("WorkBooks");
    if (!is_a_newfile_)
    {
        //��xls��Ӧ�ģ���ȡ������
        active_book_ = work_books_->querySubObject("Open(const QString&,QVariant)", xls_file_, 0);
    }
    else
    {
        //�½�һ��xls������һ���µĹ�����
        work_books_->dynamicCall("Add()");
        active_book_ = excel_instance_->querySubObject("ActiveWorkBook");
    }

    work_sheets_ = active_book_->querySubObject("WorkSheets");

    //�����Ѵ�
    loadSheet(curr_sheet_);

    is_open_ = true;
    return is_open_;
}

/**
  *@brief ����������ݣ�������д���ļ�
  */
void BikoQtExcelEngine::save()
{
    if ( active_book_ )
    {
        if (is_save_already_)
        {
            return ;
        }

        if (!is_a_newfile_)
        {
            active_book_->dynamicCall("Save()");
        }
        else     /*������ĵ����½������ģ���ʹ������ΪCOM�ӿ�*/
        {
            active_book_->dynamicCall("SaveAs (const QString&,int,const QString&,const QString&,bool,bool)",
                                      xls_file_, 56, QString(""), QString(""), false, false);

        }

        is_save_already_ = true;
    }
}

/**
  *@brief �ر�ǰ�ȱ������ݣ�Ȼ��رյ�ǰExcel COM���󣬲��ͷ��ڴ�
  */
void BikoQtExcelEngine::close()
{
    //�ر�ǰ�ȱ�������
    save();
    if ( excel_instance_ && active_book_ )
    {
        active_book_->dynamicCall("Close(bool)", true);

        is_open_     = false;
        is_valid_    = false;
        is_a_newfile_ = false;
        is_save_already_ = true;
    }
}

//
int BikoQtExcelEngine::sheetsCount()
{
    return work_books_->property("Count").toInt();
}

//�õ�ĳ��sheet������
bool BikoQtExcelEngine::sheetName(int sheet_index, QString &sheet_name)
{
    QAxObject *sheet_tmp = active_book_->querySubObject("WorkSheets(int)", sheet_index);
    if (!sheet_tmp)
    {
        return false;
    }
    sheet_name = sheet_tmp->property("Name").toString();
    return true;
}

bool BikoQtExcelEngine::loadSheet(int sheet_index)
{
    active_sheet_ = active_book_->querySubObject("WorkSheets(int)", sheet_index);

    //���û�д򿪣�
    if (!active_sheet_)
    {
        return false;
    }
    load_sheet_internal();
    return true;
}



//������ż���Sheet����,
bool BikoQtExcelEngine::loadSheet(const QString &sheet_name)
{
    active_sheet_ = active_book_->querySubObject("WorkSheets(QString)", sheet_name);
    //���û�д򿪣�
    if (!active_sheet_)
    {
        return false;
    }
    load_sheet_internal();
    return true;
}

bool BikoQtExcelEngine::hasSheet(const QString &sheet_name)
{
    QAxObject *temp_sheet = active_book_->querySubObject("WorkSheets(QString)", sheet_name);
    if (!temp_sheet)
    {
        return false;
    }
    return true;
}

void BikoQtExcelEngine::load_sheet_internal()
{
    //��ȡ��sheet��ʹ�÷�Χ����
    QAxObject *used_range = active_sheet_->querySubObject("UsedRange");
    QAxObject *rows = used_range->querySubObject("Rows");
    QAxObject *columns = used_range->querySubObject("Columns");

    //��Ϊexcel���Դ��������������ݶ���һ���Ǵ�0,0��ʼ�����Ҫ��ȡ�������±�
    //��һ�е���ʼλ��
    start_row_ = used_range->property("Row").toInt();
    //��һ�е���ʼλ��
    start_column_ = used_range->property("Column").toInt();
    //��ȡ����
    row_count_ = rows->property("Count").toInt();
    //��ȡ����
    column_count_ = columns->property("Count").toInt();
    return;
}


//!�򿪵�xls�ļ�����
QString BikoQtExcelEngine::open_filename() const
{
    return xls_file_;
}

/**
  *@brief ��tableWidget�е����ݱ��浽excel��
  *@param tableWidget : ָ��GUI�е�tablewidgetָ��
  *@return ����ɹ���� true : �ɹ�
  *                  false: ʧ��
  */
bool BikoQtExcelEngine::writeTableWidget(QTableWidget *tableWidget)
{
    if ( NULL == tableWidget )
    {
        return false;
    }
    if ( !is_open_ )
    {
        return false;
    }

    int tableR = tableWidget->rowCount();
    int tableC = tableWidget->columnCount();

    //��ȡ��ͷд����һ��
    for (int i = 0; i < tableC; i++)
    {
        if ( tableWidget->horizontalHeaderItem(i) != NULL )
        {
            this->set_cell(1, i + 1, tableWidget->horizontalHeaderItem(i)->text());
        }
    }

    //д����
    for (int i = 0; i < tableR; i++)
    {
        for (int j = 0; j < tableC; j++)
        {
            if ( tableWidget->item(i, j) != NULL )
            {
                this->set_cell(i + 2, j + 1, tableWidget->item(i, j)->text());
            }
        }
    }

    //����
    save();

    return true;
}

/**
  *@brief ��ָ����xls�ļ��а����ݵ��뵽tableWidget��
  *@param tableWidget : ִ��Ҫ���뵽��tablewidgetָ��
  *@return ����ɹ���� true : �ɹ�
  *                   false: ʧ��
  */
bool BikoQtExcelEngine::readTableWidget(QTableWidget *tableWidget)
{
    if ( NULL == tableWidget )
    {
        return false;
    }

    //�Ȱ�table���������
    int tableColumn = tableWidget->columnCount();
    tableWidget->clear();
    for (int n = 0; n < tableColumn; n++)
    {
        tableWidget->removeColumn(0);
    }

    int rowcnt    = start_row_ + row_count_;
    int columncnt = start_column_ + column_count_;

    //��ȡexcel�еĵ�һ��������Ϊ��ͷ
    QStringList headerList;
    for (int n = start_column_; n < columncnt; n++ )
    {
        QAxObject *cell = active_sheet_->querySubObject("Cells(int,int)", start_row_, n);
        if ( cell )
        {
            headerList << cell->dynamicCall("Value2()").toString();
        }
    }

    //���´�����ͷ
    tableWidget->setColumnCount(column_count_);
    tableWidget->setHorizontalHeaderLabels(headerList);


    //����������
    for (int i = start_row_ + 1, r = 0; i < rowcnt; i++, r++ )   //��
    {
        tableWidget->insertRow(r); //��������
        for (int j = start_column_, c = 0; j < columncnt; j++, c++ )   //��
        {
            QAxObject *cell = active_sheet_->querySubObject("Cells(int,int)", i, j ); //��ȡ��Ԫ��

            //��r������������������
            if ( cell )
            {
                tableWidget->setItem(r, c, new QTableWidgetItem(cell->dynamicCall("Value2()").toString()));
            }
        }
    }

    return true;
}


QVariant BikoQtExcelEngine::get_cell(int row, int column)
{
    QVariant data;

    QAxObject *cell = active_sheet_->querySubObject("Cells(int,int)", row, column); //��ȡ��Ԫ�����
    if ( cell )
    {
        data = cell->dynamicCall("Value2()");
    }

    return data;
}

/**
  *@brief �޸�ָ����Ԫ�������
  *@param row : ��Ԫ����к�
  *@param column : ��Ԫ��ָ�����к�
  *@param data : ��Ԫ��Ҫ�޸�Ϊ��������
  *@return �޸��Ƿ�ɹ� true : �ɹ�
  *                   false: ʧ��
  */
bool BikoQtExcelEngine::set_cell(int row, int column, const QVariant &data)
{
    bool op = false;

    QAxObject *cell = active_sheet_->querySubObject("Cells(int,int)", row, column); //��ȡ��Ԫ�����
    if ( cell )
    {
        QString strData = data.toString(); //excel ��Ȼֻ�ܲ����ַ��������ͣ��������޷�����
        cell->dynamicCall("SetValue(const QVariant&)", strData); //�޸ĵ�Ԫ�������
        op = true;
    }
    else
    {
        op = false;
    }

    return op;
}

/**
  *@brief ��ճ�����֮�������
  */
void BikoQtExcelEngine::clear()
{
    xls_file_     = "";
    row_count_    = 0;
    column_count_ = 0;
    start_row_    = 0;
    start_column_ = 0;
}

/**
  *@brief �ж�excel�Ƿ��ѱ���
  *@return true : �Ѵ�
  *        false: δ��
  */
bool BikoQtExcelEngine::is_open()
{
    return is_open_;
}

/**
  *@brief �ж�excel COM�����Ƿ���óɹ���excel�Ƿ����
  *@return true : ����
  *        false: ������
  */
bool BikoQtExcelEngine::is_valid()
{
    return is_valid_;
}

/**
  *@brief ��ȡexcel������
  */
int BikoQtExcelEngine::row_count()const
{
    return row_count_;
}

/**
  *@brief ��ȡexcel������
  */
int BikoQtExcelEngine::column_count()const
{
    return column_count_;
}

//
void BikoQtExcelEngine::insertSheet(const QString &sheet_name)
{
    work_sheets_->querySubObject("Add()");
    QAxObject *a = work_sheets_->querySubObject("Item(int)", 1);
    a->setProperty("Name", sheet_name);
    active_sheet_ = a;

    load_sheet_internal();
}


//ȡ���е����ƣ�����27->AA
char *BikoQtExcelEngine::column_name(int column_no)
{
    static char column_name[64];
    size_t str_len = 0;

    while (column_no > 0)
    {
        int num_data = column_no % 26;
        column_no /= 26;
        if (num_data == 0)
        {
            num_data = 26;
            column_no--;
        }
        //��֪������Բ���
        column_name[str_len] = (char)((num_data - 1) + ('A'));
        str_len++;
    }
    column_name[str_len] = '\0';
    //��ת
    strrev(column_name);

    return column_name;
}