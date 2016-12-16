//
// Created by mwo on 16/12/16.
//

#ifndef RESTBED_XMR_MYSQLACCOUNTS_H
#define RESTBED_XMR_MYSQLACCOUNTS_H


#include "tools.h"
#include "ssqlses.h"
#include "MySqlConnector.h"

#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>


#include <iostream>
#include <memory>


namespace xmreg
{

using namespace mysqlpp;
using namespace std;
using namespace nlohmann;




class MysqlOutpus
{

    shared_ptr<MySqlConnector> conn;

public:

    MysqlOutpus(shared_ptr<MySqlConnector> _conn): conn {_conn}
    {}

    bool
    select(const uint64_t& address_id, vector<XmrOutput>& outs)
    {
//
//        static shared_ptr<Query> query;
//
//        if (!query)
//        {
//            Query q = MySqlConnector::getInstance().query(
//                    XmrTransaction::SELECT_STMT);
//            q.parse();
//            query = shared_ptr<Query>(new Query(q));
//        }

        Query query = conn->query(XmrOutput::SELECT_STMT);
        query.parse();

        try
        {
            query.storein(outs, address_id);

            if (!outs.empty())
            {
                return true;
            }
        }
        catch (mysqlpp::Exception& e)
        {
            MYSQL_EXCEPTION_MSG(e);
        }
        catch (std::exception& e)
        {
            MYSQL_EXCEPTION_MSG(e);
        }

        return false;
    }


    uint64_t
    insert(const XmrOutput& out_data)
    {

//        static shared_ptr<Query> query;
//
//        if (!query)
//        {
//            Query q = MySqlConnector::getInstance().query(XmrOutput::INSERT_STMT);
//            q.parse();
//            query = shared_ptr<Query>(new Query(q));
//        }


        Query query = conn->query(XmrOutput::INSERT_STMT);
        query.parse();

        // cout << query << endl;

        try
        {
            SimpleResult sr = query.execute(out_data.account_id,
                                            out_data.tx_id,
                                            out_data.out_pub_key,
                                            out_data.tx_pub_key,
                                            out_data.out_index,
                                            out_data.mixin,
                                            out_data.timestamp);

            if (sr.rows() == 1)
                return sr.insert_id();

        }
        catch (mysqlpp::Exception& e)
        {
            MYSQL_EXCEPTION_MSG(e);
            return 0;
        }

        return 0;
    }

};



class MysqlTransactions
{

    shared_ptr<MySqlConnector> conn;

public:

    MysqlTransactions(shared_ptr<MySqlConnector> _conn): conn {_conn}
    {}

    bool
    select(const uint64_t& address_id, vector<XmrTransaction>& txs)
    {
//
//        static shared_ptr<Query> query;
//
//        if (!query)
//        {
//            Query q = MySqlConnector::getInstance().query(
//                    XmrTransaction::SELECT_STMT);
//            q.parse();
//            query = shared_ptr<Query>(new Query(q));
//        }

        Query query = conn->query(XmrTransaction::SELECT_STMT);
        query.parse();

        try
        {
            query.storein(txs, address_id);

            if (!txs.empty())
            {
                return true;
            }
        }
        catch (mysqlpp::Exception& e)
        {
            MYSQL_EXCEPTION_MSG(e);
        }
        catch (std::exception& e)
        {
            MYSQL_EXCEPTION_MSG(e);
        }

        return false;
    }


    uint64_t
    insert(const XmrTransaction& tx_data)
    {

//        static shared_ptr<Query> query;
//
//        if (!query)
//        {
//            Query q = MySqlConnector::getInstance().query(XmrTransaction::INSERT_STMT);
//            q.parse();
//            query = shared_ptr<Query>(new Query(q));
//        }


        Query query = conn->query(XmrTransaction::INSERT_STMT);
        query.parse();

        // cout << query << endl;

        try
        {
            SimpleResult sr = query.execute(tx_data.hash,
                                            tx_data.account_id,
                                            tx_data.total_received,
                                            tx_data.total_sent,
                                            tx_data.unlock_time,
                                            tx_data.height,
                                            tx_data.coinbase,
                                            tx_data.payment_id,
                                            tx_data.mixin,
                                            tx_data.timestamp);

            if (sr.rows() == 1)
                return sr.insert_id();

        }
        catch (mysqlpp::Exception& e)
        {
            MYSQL_EXCEPTION_MSG(e);
            return 0;
        }

        return 0;
    }

    uint64_t
    get_total_recieved(const uint64_t& account_id)
    {
        Query query = conn->query(XmrTransaction::SUM_XMR_RECIEVED);
        query.parse();

        try
        {
            StoreQueryResult sqr = query.store(account_id);

            if (!sqr)
            {
                return 0;
            }

            Row row = sqr.at(0);

            return row["total_received"];
        }
        catch (mysqlpp::Exception& e)
        {
            MYSQL_EXCEPTION_MSG(e);
            return 0;
        }
    }


};


class MySqlAccounts
{

    shared_ptr<MySqlConnector> conn;

    shared_ptr<MysqlTransactions> mysql_tx;

    shared_ptr<MysqlOutpus> mysql_out;

public:


    MySqlAccounts()
    {
        //cout << "MySqlAccounts() makes new connection" << endl;
        conn      = make_shared<MySqlConnector>();
        mysql_tx  = make_shared<MysqlTransactions>(conn);
        mysql_out = make_shared<MysqlOutpus>(conn);
    }


    bool
    select(const string& address, XmrAccount& account)
    {

//        static shared_ptr<Query> query;
//
//        if (!query)
//        {
//            Query q = MySqlConnector::getInstance().query(XmrAccount::SELECT_STMT);
//            q.parse();
//            query = shared_ptr<Query>(new Query(q));
//        }

        Query query = conn->query(XmrAccount::SELECT_STMT);
        query.parse();

        try
        {
            vector<XmrAccount> res;
            query.storein(res, address);

            if (!res.empty())
            {
                account = res.at(0);
                return true;
            }

        }
        catch (mysqlpp::Exception& e)
        {
            MYSQL_EXCEPTION_MSG(e);
        }
        catch (std::exception& e)
        {
            MYSQL_EXCEPTION_MSG(e);
        }

        return false;
    }

    bool
    select(const int64_t& acc_id, XmrAccount& account)
    {

        //static shared_ptr<Query> query;

//        if (!query)
//        {
//            Query q = MySqlConnector::getInstance().query(XmrAccount::SELECT_STMT2);
//            q.parse();
//            query = shared_ptr<Query>(new Query(q));
//        }

        Query query = conn->query(XmrAccount::SELECT_STMT2);
        query.parse();

        try
        {
            vector<XmrAccount> res;
            query.storein(res, acc_id);

            if (!res.empty())
            {
                account = res.at(0);
                return true;
            }

        }
        catch (mysqlpp::Exception& e)
        {
            MYSQL_EXCEPTION_MSG(e);
        }

        return false;
    }

    uint64_t
    insert(const string& address, const uint64_t& scanned_block_height = 0)
    {

        //    static shared_ptr<Query> query;

//        if (!query)
//        {
//            Query q = MySqlConnector::getInstance().query(XmrAccount::INSERT_STMT);
//            q.parse();
//            query = shared_ptr<Query>(new Query(q));
//        }


        Query query = conn->query(XmrAccount::INSERT_STMT);
        query.parse();

        // cout << query << endl;

        try
        {
            SimpleResult sr = query.execute(address, scanned_block_height);

            if (sr.rows() == 1)
                return sr.insert_id();

        }
        catch (mysqlpp::Exception& e)
        {
            MYSQL_EXCEPTION_MSG(e);
            return 0;
        }

        return 0;
    }

    uint64_t
    insert_tx(const XmrTransaction& tx_data)
    {
        return mysql_tx->insert(tx_data);
    }

    uint64_t
    insert_output(const XmrOutput& tx_out)
    {
        return mysql_out->insert(tx_out);
    }

    bool
    select_txs(const string& xmr_address, vector<XmrTransaction>& txs)
    {
        // having address first get its address_id


        // a placeholder for exciting or new account data
        xmreg::XmrAccount acc;

        // select this account if its existing one
        if (!select(xmr_address, acc))
        {
            cerr << "Address" << xmr_address << "does not exist in database" << endl;
            return false;
        }

         return mysql_tx->select(acc.id, txs);
    }

    bool
    select_txs(const uint64_t& account_id, vector<XmrTransaction>& txs)
    {
        return mysql_tx->select(account_id, txs);
    }


    bool
    select_outputs(const uint64_t& account_id, vector<XmrOutput>& outs)
    {
        return mysql_out->select(account_id, outs);
    }


    uint64_t
    get_total_recieved(const uint64_t& account_id)
    {
        return mysql_tx->get_total_recieved(account_id);
    }


    bool
    update(XmrAccount& acc_orginal, XmrAccount& acc_new)
    {

        Query query = conn->query();

        try
        {
            query.update(acc_orginal, acc_new);

            SimpleResult sr = query.execute();

            if (sr.rows() == 1)
                return true;
        }
        catch (mysqlpp::Exception& e)
        {
            MYSQL_EXCEPTION_MSG(e);
            return false;
        }

        return false;
    }

};


}


#endif //RESTBED_XMR_MYSQLACCOUNTS_H
