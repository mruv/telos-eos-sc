#pragma once 

#include <string>

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/symbol.hpp>

using eosio::name;
using eosio::contract;
using eosio::asset;
using eosio::multi_index;
using eosio::datastream;
using eosio::permission_level;
using eosio::transaction;
using eosio::action;
using eosio::symbol;
using eosio::symbol_code;

namespace SmtsBusiness {

	class [[eosio::contract("smtsbusiness")]] SmtsBusiness : public contract {

		public:
			SmtsBusiness(name self, name code, datastream<const char*> ds) : contract(self, code, ds) {}

			[[eosio::action("createcurr")]]   void CreateToken(const asset& max_supply);
			[[eosio::action("createcomm")]]   void CreateCmmdty(const name& issuer, const asset& max_supply, const asset& unit_price);
			[[eosio::action("issue")]]        void Issue(const name& to, const asset& quantity, const std::string& memo);
			[[eosio::action("transfer")]]     void Transfer(const name& from, const name& to, const asset& quantity, const std::string& memo);
			[[eosio::action("inventoryreq")]] void InvRequest(const name& from, const name& to, const asset& quantity);
			[[eosio::action("fulfill")]]      void Fulfill(const name& merchant_acct, uint64_t ir_id);
			[[eosio::action("pay")]]          void Pay(const name& customerAcct, uint64_t irId);

			[[eosio::action("deletedata")]]   void DeleteData();

		private:

			void Create(const name& issuer, const asset& max_supply);
			void UpdateDestAcct(const name& from, const name& to, const asset& value);
			void UpdateSrcAcct(const name& owner, const asset& value);
			
			struct [[eosio::table("account")]] Account {
				asset    balance;

				uint64_t primary_key() const { return balance.symbol.raw(); }
			};

			struct [[eosio::table("assetstats")]] AssetStats {
				asset    supply;
				asset    max_supply;
				name     issuer;

				uint64_t primary_key() const { return supply.symbol.code().raw(); }
			};

			struct [[eosio::table("cunitprice")]] CommodityUnitPrice {
				symbol_code comm_name;
				asset       unit_price;

				uint64_t primary_key() const { return comm_name.raw(); }
			};

			struct [[eosio::table("inventoryreq")]] InventoryReq {
				uint64_t    id;
				name        from;
				name        to;
				asset       quantity;
				asset       unit_price;
				std::string status;

				uint64_t primary_key() const { return id; }
			};

			// Table configuration
			typedef multi_index<"accounts"_n, Account>               Accounts;
			typedef multi_index<"stat"_n, AssetStats>                Stats;
			typedef multi_index<"cunitprices"_n, CommodityUnitPrice> CommodityUnitPrices;
			typedef multi_index<"inventreqs"_n, InventoryReq>        InventoryReqs;
	};
}