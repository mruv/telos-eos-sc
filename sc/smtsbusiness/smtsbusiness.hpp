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

	// Scrap Metals Business EOS Smart Contract.
	// 
	// A platform on which a scrap metal merchant can interact with a scrap metal customer
	// in a secure and efficient way.
	//
	// A scrap metal, in the context of this smart contract, is a EOS token / asset (in this case a Commodity) that can be transfered
	// from one EOS acct to another, in exchange for another EOS  token / asset (a Currency).
	//
	// The units for all Commodities (EOS tokens representing a basic good used in commerce) is pounds (lbs).
	// Therefore, "10.0 IRON" means 10.0 lbs of IRON.
	// 
	// A currency, in the context of this smart contract, is a EOS token that 
	class [[eosio::contract("smtsbusiness")]] SmtsBusiness : public contract {

		public:
			SmtsBusiness(name self, name code, datastream<const char*> ds) : contract(self, code, ds) {}

			[[eosio::action("createcurr")]]   void CreateToken(const asset& maxSupply);
			[[eosio::action("createcomm")]]   void CreateCmmdty(const name& issuer, const asset& maxSupply, const asset& unitPrice);
			[[eosio::action("issue")]]        void Issue(const name& to, const asset& quantity, const std::string& memo);
			[[eosio::action("transfer")]]     void Transfer(const name& from, const name& to, const asset& quantity, const std::string& memo);
			[[eosio::action("inventoryreq")]] void InvRequest(const name& from, const name& to, const asset& quantity);
			[[eosio::action("fulfill")]]      void Fulfill(const name& merchantAcct, uint64_t irId);
			[[eosio::action("pay")]]          void Pay(const name& customerAcct, uint64_t irId);
			[[eosio::action("upsertmsl")]]    void UpsertMsl(const name& payer, const asset& commodity);

			[[eosio::action("deletedata")]]   void DeleteData();

		private:

			void Create(const name& issuer, const asset& maxSupply);
			void AddBalance(const name& from, const name& to, const asset& value);
			void SubBalance(const name& owner, const asset& value);
			
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

			// Minimum Stock Levels
			// Specifies the minimun amount of a certain comodity (in this context, scrap metal) that should trigger
			// an inventory request
			struct [[eosio::table("minstocklev")]] MinStockLevel {
				asset supply;

				uint64_t primary_key() const { return supply.symbol.code().raw(); } 
			};

			// Table configuration
			typedef multi_index<"accounts"_n, Account>               Accounts;
			typedef multi_index<"stat"_n, AssetStats>                Stats;
			typedef multi_index<"cunitprices"_n, CommodityUnitPrice> CommodityUnitPrices;
			typedef multi_index<"inventreqs"_n, InventoryReq>        InventoryReqs;
			typedef multi_index<"minstocklev"_n, MinStockLevel>      MinStockLevels;
	};
}