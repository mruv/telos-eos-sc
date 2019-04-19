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
	// A currency, in the context of this smart contract, is a EOS token that is used as a medium of exchange.
	// This SC has one currency / token, YELOS and it's created and issued by the contract owner.
	class [[eosio::contract("smtsbusiness")]] SmtsBusiness : public contract {

		public:
			SmtsBusiness(name self, name code, datastream<const char*> ds) : contract(self, code, ds) {}

			// Creates a New Token.
			//
			// @param maxSupply - an asset variable that defines the total amount of YELOS all the participants can hold
			[[eosio::action("createcurr")]]   void CreateToken(const asset& maxSupply);

			// Creates a commodity - a type of a scrap metal
			//
			// @param issuer    - the EOS acct associated with the creator of the token
			// @param maxSupply - an asset variable that defines the total amount of YELOS all the participants can hold
			// @param unitPrice - an asset defining the amount of YELOS that each unit (default unit is pounds) of a certain commodity
			[[eosio::action("createcomm")]]   void CreateCmmdty(const name& issuer, const asset& maxSupply, const asset& unitPrice);

			// Distributes a token.
			//
			// @param to       - source EOS acct of the token
			// @param quantity - amount of a certain token to be allocated to a certain EOS acct
			// @param memo     - a short description of the transaction
			[[eosio::action("issue")]]        void Issue(const name& to, const asset& quantity, const std::string& memo);

			// Moves a specified number of tokens from one acct to another.
			// 
			// @param from     - source EOS acct name
			// @param to       - destination EOS acct name
			// @param quantity - amount to transfer
			// @param memo     - a short description of the transaction
			[[eosio::action("transfer")]]     void Transfer(const name& from, const name& to, const asset& quantity, const std::string& memo);

			// Sends a inventory request to a scrap metal dealer.
			//
			// @param from     - EOS acct name of the customer placing the order
			// @param to       - EOS acct name of the merchant receiving the order
			// @param quantity - amount needed
			[[eosio::action("inventoryreq")]] void InvRequest(const name& from, const name& to, const asset& quantity);

			// Fulfills an inventory request by transfering the required amount of tokens to the customer's EOS acct
			//
			// @param merchantAcct - EOS acct name of the merchant receiving the order
			// @param irId         - The Primary Key for the inventory request
			[[eosio::action("fulfill")]]      void Fulfill(const name& merchantAcct, uint64_t irId);

			// Settles a fulfilled inventory request by transfering the appropriate amount of YELOS from a customers acct to the
			// scrap metal merchant's acct.
			//
			// @param customerAcct - EOS acct name of the customer who placed the order
			// @param irId         - The Primary Key for the inventory request
			[[eosio::action("pay")]]          void Pay(const name& customerAcct, uint64_t irId);

			// Minimum Stock Levels - The minimum balance for a certain commodity (a token representing a certain type of a scrap metal) that's
			// needed to automatically trigger an Inventory Request.
			//
			// Creates / Updates Minimum Stock Level for a specified commodity.
			//
			// @param payer      - EOS acct name of the customer.
			// @param supplier   - EOS acct name of the merchant to receive the automated Inventory Requests.
			// @param minBal     - The minimum asset / token / commodity balance (This is in pounds) required to trigger an nventory Request
			// @param invreqAmnt - Number / amount of pounds worth of a specified commodity to be ordered automatically. 
			[[eosio::action("upsertmsl")]]    void UpsertMsl(const name& payer, const name& supplier, const asset& minBal, const asset& invreqAmnt);

			// Burns a commodity (token) in order to simulate stock sell out. This is the ACTION that triggers the automated inventory requests.
			//
			// @param payer    - EOS acct name of the holder of a specified token (commodity).
			// @param quantity - Amount to burn 
			[[eosio::action("sell")]]         void Sell(const name& payer, const asset& quantity);

			// Clears the Smart Contract Data -- All the tables. USE WISELY.
			[[eosio::action("deletedata")]]   void DeleteData();

		private:

			// Helper methods.
			void Create(const name& issuer, const asset& maxSupply);
			void AddBalance(const name& from, const name& to, const asset& value);
			void SubBalance(const name& owner, const asset& value);
			
			struct [[eosio::table("account")]] Account {
				asset    balance;

				uint64_t primary_key() const { return balance.symbol.code().raw(); }
			};

			// Keeps record about the amount of a certain token that's in supply.
			struct [[eosio::table("assetstats")]] AssetStats {
				asset    supply;
				asset    max_supply;
				name     issuer;

				uint64_t primary_key() const { return supply.symbol.code().raw(); }
			};

			// Defines the value of a commodity against YELOS
			struct [[eosio::table("cunitprice")]] CommodityUnitPrice {
				symbol_code comm_name;
				// YELOS / lbs
				asset       unit_price;

				uint64_t primary_key() const { return comm_name.raw(); }
			};

			// Inventory Requests
			struct [[eosio::table("inventoryreq")]] InventoryReq {
				uint64_t    id;
				// customer
				name        from;
				// merchant
				name        to;
				asset       quantity;
				// YELOS / lbs
				asset       unit_price;
				// PENDING -> FULFILLED -> PAID
				std::string status;

				uint64_t primary_key() const { return id; }
			};

			// Minimum Stock Levels
			// Specifies the minimun amount of a certain comodity (in this context, scrap metal) that should trigger
			// an inventory request
			struct [[eosio::table("minstocklev")]] MinStockLevel {
				asset min_balance;
				asset invreq_amnt;
				name  supplier;

				uint64_t primary_key() const { return min_balance.symbol.code().raw(); } 
			};

			// Table configuration
			typedef multi_index<"accounts"_n, Account>               Accounts;
			typedef multi_index<"stat"_n, AssetStats>                Stats;
			typedef multi_index<"cunitprices"_n, CommodityUnitPrice> CommodityUnitPrices;
			typedef multi_index<"inventreqs"_n, InventoryReq>        InventoryReqs;
			typedef multi_index<"minstocklevs"_n, MinStockLevel>     MinStockLevels;
	};
}