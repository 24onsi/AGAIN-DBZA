#include <iostream>
#include <mariadb/conncpp.hpp>
#include <string>
#include <vector>
#include <ctime>
using namespace std;

class DB
{
private:
    string m_id;
    string m_pw;
    sql::Connection* m_connection;

public:
    DB() { }
    void account(const string& id, const string& pw)
    {
        m_id = id;
        m_pw = pw;
    }

    sql::PreparedStatement* prepareStatement(const string& query)
    {
        sql::PreparedStatement* stmt(m_connection->prepareStatement(query));
        return stmt;
    }
    
    void connect()
    {
        try{
            sql::Driver* driver = sql::mariadb::get_driver_instance();
            sql::SQLString url = "jdbc:mariadb://10.10.21.114/agdbza";    // db의 주소
            sql::Properties properties({{"user", m_id}, {"password", m_pw}});
            m_connection = driver->connect(url,properties);
            cout << "DB 접속\n";  
        }catch(sql::SQLException& e){
            cerr<<"DB 접속 실패: " << e.what() << endl;
        }
    }

    ~DB() { cerr<< "bye~\n";}
};

class Info
{
private:
    DB& db;

public:
    Info(DB& idb): db(idb) {}

    void show_local()
    {
        try
        {
            sql::PreparedStatement* travel = db.prepareStatement("select distinct t_loc from travel");

            sql::ResultSet *res  = travel->executeQuery();
            while(res->next())
            {
                cout << " # "<< res->getString(1) << endl;
            }
        }
        catch(sql::SQLException& e)
        {
            std::cerr << "Error deleting task: " << e.what() << std::endl;
        }
    }

    void show_List(string loc)
    {
        try
        {
            sql::PreparedStatement* travel = db.prepareStatement("select * from travel where t_loc = ?");
            travel->setString(1, loc);

            sql::ResultSet *res  = travel->executeQuery();
            cout << endl;
            while(res->next())
            {
                cout << " # 여행지 코드  [ "<< res->getInt(1) << " ] " << endl;
                cout << "   여 행 지 : " << res->getString(2) << endl;
                cout << "   주    소 : " << res->getString(4);
                cout << "   연 락 처 : " << res->getString(5) << endl << endl;
            }
        }
        catch(sql::SQLException& e)
        {
            std::cerr << "Error deleting task: " << e.what() << std::endl;
        }
    }

    void travelname(int code)
    {
        try
        {
            sql::PreparedStatement* travel = db.prepareStatement("select t_name from travel where t_id = ?");
            travel->setInt(1, code);

            sql::ResultSet *res  = travel->executeQuery();
            while(res->next())
            {
                cout << res->getString(1);
            }
        }
        catch(sql::SQLException& e)
        {
            std::cerr << "Error deleting task: " << e.what() << std::endl;
        } 
    }

    void info(int code)
    {
        try
        {
            sql::PreparedStatement* travel = db.prepareStatement("select t_name, t_addr, t_telenum from travel where t_id = ?");
            travel->setInt(1, code);

            sql::ResultSet *res  = travel->executeQuery();
            cout << " =============================================================================" << endl << endl;
            while(res->next())
            {
                cout << " | " << res->getString(1) << endl;
                cout << "   주    소 : " << res->getString(2);
                cout << "   연 락 처 : " << res->getString(3) << endl << endl;
            }
            
            sql::PreparedStatement* fes = db.prepareStatement("select f_name, start, end from travel inner join festival on travel.fes_id = festival.f_id where t_id = ?");
            fes->setInt(1, code);

            sql::ResultSet *fres = fes->executeQuery();
            cout << " | 축  제  정  보" << endl;
            while(fres->next())
            {
                cout << "   축 제 명 : " << fres->getString(1) << endl;
                cout << "   기    간 : " << fres->getString(2) << " ~ " << fres->getString(3) << endl << endl;
            }

            sql::PreparedStatement* acc = db.prepareStatement("select a_type, a_times, a_emr from travel inner join accident on travel.acc_id = accident.a_id where t_id = ?");
            acc->setInt(1, code);

            sql::ResultSet *ares = acc->executeQuery();
            cout << " | 사  고  정  보" << endl;
            while(ares->next())
            {
                cout << "   사고 유형 : " << ares->getString(1) << endl;
                cout << "   발생 횟수 : " << ares->getInt(2) << endl;
                cout << "   응급 처치 : " << ares->getString(3) << endl << endl;
            }

            sql::PreparedStatement* hos = db.prepareStatement("select h_name, h_addr, h_telenum from travel inner join hospital on travel.hos_id = hospital.h_id where t_id = ?");
            hos->setInt(1, code);

            sql::ResultSet *hres = hos->executeQuery();
            cout << " | 병  원  정  보" << endl;
            while(hres->next())
            {
                cout << "   병 원 명 : " << hres->getString(1) << endl;
                cout << "   주    소 : " << hres->getString(2) << endl;
                cout << "   연 락 처 : " << hres->getString(3) << endl << endl;
            }

            sql::PreparedStatement* phar = db.prepareStatement("select p_name, p_addr, p_telenum from travel inner join pharmacy on travel.phar_id = pharmacy.p_id where t_id = ?");
            phar->setInt(1, code);

            sql::ResultSet *pres = phar->executeQuery();
            cout << " | 약  국  정  보" << endl;
            while(pres->next())
            {
                cout << "   약 국 명 : " << pres->getString(1) << endl;
                cout << "   주    소 : " << pres->getString(2) << endl;
                cout << "   연 락 처 : " << pres->getString(3) << endl << endl;
            }
            cout << " =============================================================================" << endl;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
};

class Emergency
{
private:
    DB& db;

public:
    Emergency(DB& edb): db(edb) {}

    void showchoice()
    {
        cout << endl;
        cout << " ==============================================================================" << endl;
        cout << " 1. 응급 상황 별 대처방안       2. 지역별 병원 정보       3. 지역별 약국 정보" << endl;
        cout << " ==============================================================================" << endl;
        cout << " 번호를 입력하세요. : ";
    }

    void accType()
    {
        try
        {
            sql::PreparedStatement* acc = db.prepareStatement("select distinct a_type from accident");
            sql::ResultSet *res  = acc->executeQuery();

            cout << " [ 응 급 상 황 별 ]" << endl << endl;
            while(res->next())
            {
                cout << " | " << res->getString(1);
            }
            cout << "|" << endl << endl;
        }
        catch(sql::SQLException& e)
        {
            std::cerr << "Error deleting task: " << e.what() << std::endl;
        }
    }

    void erMeasure(string type)
    {
         try
        {
            sql::PreparedStatement* er = db.prepareStatement("select a_type, a_emr from accident where a_type = ? group by a_type");
            er->setString(1, type);
            sql::ResultSet *res  = er->executeQuery();

            while(res->next())
            {
                cout << " | "<< res->getString(1) << endl;
                cout << "  응급 조치 방안 (사이트 이동) " << endl;
                cout << " " << res->getString(2) << endl;
            }
        }
        catch(sql::SQLException& e)
        {
            std::cerr << "Error deleting task: " << e.what() << std::endl;
        }
    }

    void locHospital(string loc)
    {
        try
        {
            sql::PreparedStatement* hos = db.prepareStatement("select h_name, h_addr, h_telenum from hospital where h_loc = ?");
            hos->setString(1, loc);

            sql::ResultSet *res  = hos->executeQuery();
            while(res->next())
            {
                cout << " | 병원명 : " << res->getString(1) << endl;
                cout << "   주 소 : " << res->getString(2) << " ,   연락처 : " << res->getString(3) << endl;

            }
        }
        catch(sql::SQLException& e)
        {
            std::cerr << "Error deleting task: " << e.what() << std::endl;
        }  
    }

    void locPharmacy(string loc)
    {
        try
        {
            sql::PreparedStatement* phar = db.prepareStatement("select p_name, p_addr, p_telenum from pharmacy where p_loc = ?");
            phar->setString(1, loc);

            sql::ResultSet *res  = phar->executeQuery();
            while(res->next())
            {
                cout << " | 약국명 : " << res->getString(1) << endl;
                cout << "   주 소 : " << res->getString(2) << " ,   연락처 : " << res->getString(3) << endl;

            }
        }
        catch(sql::SQLException& e)
        {
            std::cerr << "Error deleting task: " << e.what() << std::endl;
        }  
    }
};

class History
{
private:
    DB& db;
    
public:
    History(DB& hdb): db(hdb) {}
    
    vector<vector<string>> inputHistory(string loc, int code)
    {
        string date;
        sql::PreparedStatement* time = db.prepareStatement("select now()");
        sql::ResultSet *res  = time->executeQuery();
        while(res->next())
            date = res->getString(1);
        
        static vector<vector<string>>history;
        vector<string>list;
        string num = to_string(code);

        list.emplace_back(date);
        list.emplace_back(loc);
        list.emplace_back(num);

        if(history.size() >= 30)
        {
            history.emplace(history.begin(), list);
            history.erase(history.begin()+1);
        }
        else
        {
            history.emplace_back(list);
        }

        return history;
    }

    void outputHistory(vector<vector<string>> history, Info& idb)
    {
        cout  << " || 검색 기록 " << endl;
        for(int i = 0; i < history.size() ; i++)
        {
            cout << i+1 << "번 기록";
            cout << "  검색 시간 ( " << history[i][0];
            cout << " )  검색 지역 : " << history[i][1];
            cout << "  검색 여행지 : ";
            idb.travelname(stoi(history[i][2]));
            cout << endl << endl;
        }

        if(history.size() > 0)
        {
            int ch, num;
            cout << " > 재검색 원하는 번호를 입력하세요. (재검색 X : q): ";
            cin >> ch;
            if(ch <= 30)
            {
                num = stoi(history[ch-1][2]);
                idb.info(num);
            }
        }
    }
};

class handler
{
private:
    DB& db;
    string id, pw;
    vector<vector<string>>search;

public:
    handler(DB& hdb): db(hdb) {}
    
    void Login()
    {
        cout << "[   로그인    ]" << endl;
        cout << " ID : ";
        getline(cin, id);
        cout << " passward : ";
        getline(cin, pw);
        db.account(id, pw);
        db.connect();
    }

    void showLocalList()
    {
        cout << endl;
        cout << " =============================================================================" << endl;
        cout << "  1. 지역별 여행지 정보     2. 응급조치 및 지역별 의료시설 정보    3. 종 료" << endl;
        cout << " =============================================================================" << endl;
        cout << " 항목 번호를 입력하세요. : ";
    }

    void travelchoice()
    {
        cout << endl;
        cout << " ================================================" << endl;
        cout << " 1. 여행지 정보 검색       2. 검색 기록 확인" << endl;
        cout << " ================================================" << endl;
        cout << " 항목 번호를 입력하세요. : ";
    }

    void showinfo()
    {
        Info tr(db);
        Emergency em(db);
        History hi(db);
        int choice = 0, num = 0, code = 0;
        string loc, type;

        while(1)
        {
            showLocalList();
            (cin >> choice).get();
            if(choice == 1)
            {
                travelchoice();
                (cin >> num).get();
                if(num == 1)
                {
                    cout << " [  지역 별 여행지 정보  ]" << endl << endl;
                    tr.show_local();
                    cout << " 검색 지역을 입력하세요. :"; 
                    getline(cin, loc);
                    tr.show_List(loc);
                    cout << " 검색 여행지 코드를 입력하세요. ";
                    cin >> code;
                    tr.info(code);
                    search = hi.inputHistory(loc, code);
                }
                else
                {
                    hi.outputHistory(search, tr);
                }
            }
            else if(choice == 2)
            {
                em.showchoice();
                (cin >> num).get();
                if(num == 1)
                {
                    em.accType();
                    cout << " 응급상황을 입력하세요. : ";
                    getline(cin, type);
                    em.erMeasure(type);
                }
                else if(num == 2)
                {
                    cout << " 검색지역을 입력하세요. : ";
                    getline(cin, loc);
                    em.locHospital(loc);
                }
                else if(num == 3)
                {
                    cout << " 검색지역을 입력하세요. : ";
                    getline(cin, loc);
                    em.locPharmacy(loc);
                }
            }
            else
            {
                cout << "종료합니다." << endl;
                break;
            }
        }

    }
};

int main()
{

    DB db;
    handler ctl(db);

    ctl.Login();
    ctl.showinfo();

    return 0;
}
