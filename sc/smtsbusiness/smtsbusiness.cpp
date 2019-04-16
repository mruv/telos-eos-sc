
#include "smtsbusiness.hpp"

namespace SmtsBusiness {

	void SmtsBusiness::CreateToken(const asset& max_supply) {

		require_auth(_self);

		asset yelos = {0, symbol{"YELOS", 4}};
		eosio_assert(yelos.symbol.code().raw() == max_supply.symbol.code().raw(), "YELOS is the contract's only currency / token");
		Create(_self, max_supply);

		action{
			permission_level{_self,"active"_n},
			_self,
			"issue"_n,
			std::make_tuple(_self, max_supply, std::string{"Created Tokens Issued to Token Owner"})	
		}.send();
	}

	void SmtsBusiness::CreateCmmdty(const name& issuer, const asset& max_supply, const asset& unit_price) {

		require_auth(issuer);

		Stats commodityStats(_self, unit_price.symbol.code().raw());
		auto existing_stats_iter = commodityStats.find(unit_price.symbol.code().raw());
		eosio_assert(existing_stats_iter != commodityStats.end(), "specified currency doesn't exist");

		Create(issuer, max_supply);
		CommodityUnitPrices commUnitPrices(_self, issuer.value);
		auto existing_smrts_iter = commUnitPrices.find(max_supply.symbol.code().raw());

		if (existing_smrts_iter == commUnitPrices.end()) {
			// create new
			commUnitPrices.emplace(issuer, [&](auto& smrate) {
				smrate.comm_name  = max_supply.symbol.code();
				smrate.unit_price = unit_price;
			});

			action {
				permission_level {issuer,"active"_n},
				_self,
				"issue"_n,
				std::make_tuple(issuer, max_supply, std::string{"Created Tokens Issued to Token Owner"})	
			}.send();

		}  else {
			// update
			commUnitPrices.modify(existing_smrts_iter, issuer, [&](auto& smrate) {
				smrate.unit_price = unit_price;
			});
		}
	}

	void SmtsBusiness::Create(const name& issuer, const asset& max_supply) {

		auto asset_sym = max_supply.symbol;
		eosio_assert(asset_sym.is_valid(), "invalid symbol name" );
		eosio_assert(max_supply.is_valid(), "invalid supply");
		eosio_assert(max_supply.amount > 0, "max-supply must be positive");

		Stats assetStats(_self, asset_sym.code().raw());
		auto existing = assetStats.find(asset_sym.code().raw());
		eosio_assert(existing == assetStats.end(), "token with symbol already exists");

		// create a new asset
		assetStats.emplace(_self, [&](auto& s) {
		   s.supply.symbol = max_supply.symbol;
		   s.max_supply    = max_supply;
		   s.issuer        = issuer;
		});
	}

	void SmtsBusiness::Issue(const name& to, const asset& quantity, const std::string& memo) {

		auto asset_sym = quantity.symbol;
	    eosio_assert(asset_sym.is_valid(), "invalid symbol name");
	    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

	    Stats tokenStats(_self, asset_sym.code().raw());
	    auto existing = tokenStats.find(asset_sym.code().raw());
	    eosio_assert(existing != tokenStats.end(), "asset with symbol does not exist, create token before Issue");

	    require_auth(existing->issuer);
	    eosio_assert(quantity.is_valid(), "invalid quantity");
	    eosio_assert(quantity.amount > 0, "must Issue positive quantity");

	    eosio_assert(quantity.symbol == existing->supply.symbol, "symbol precision mismatch");
	    eosio_assert(quantity.amount <= existing->max_supply.amount - existing->supply.amount, "quantity exceeds available supply");

	    tokenStats.modify(*existing, existing->issuer, [&](auto& s) {
	       s.supply += quantity;
	    });

	    UpdateDestAcct(existing->issuer, existing->issuer, quantity);

	    if(to != existing->issuer ) {
	    	action inline_action = action(
	    		permission_level{existing->issuer,"active"_n},
	    		_self,
			    "transfer"_n,
			    std::make_tuple(existing->issuer, to, quantity, memo)
			  );

	    	inline_action.send();
	    }
	}

	void SmtsBusiness::Transfer(const name& from, const name& to, const asset& quantity, const std::string& memo) {

		eosio_assert(from != to, "cannot Transfer to self" );
		// sign transaction using 'from'
		require_auth(from);
		// receiver must exist
		eosio_assert(is_account(to), "to account does not exist");
		auto asset_sym = quantity.symbol;
		Stats tokenStats(_self, asset_sym.code().raw());
		const auto& st = tokenStats.get(asset_sym.code().raw());

		// notify both the sender and receiver
		require_recipient(from);
		require_recipient(to);

		eosio_assert(quantity.is_valid(), "invalid quantity" );
		eosio_assert(quantity.amount > 0, "must Transfer positive quantity" );
		eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
		eosio_assert(memo.size() <= 256, "memo has more than 256 bytes" );

		// update sender balance
		UpdateSrcAcct(from, quantity);
		// update receiver balance
		UpdateDestAcct(from, to, quantity);
	}

	void SmtsBusiness::UpdateDestAcct(const name& payer, const name& to, const asset& value) {

		Accounts accounts(_self, to.value);
		auto rcvr_acc = accounts.find(value.symbol.raw());

		if(rcvr_acc == accounts.end()) {
			accounts.emplace(payer, [&](auto& a) {
				a.balance = value;
			});
		} else {
			accounts.modify(rcvr_acc, payer, [&](auto& a) {
				a.balance += value;
			});
		}
	}

	void SmtsBusiness::UpdateSrcAcct(const name& payer, const asset& value) {

		Accounts accounts(_self, payer.value);
		const auto& sndr_acc = accounts.get(value.symbol.raw(), "no balance object found");
		eosio_assert(sndr_acc.balance.amount >= value.amount, "overdrawn balance");

		if(sndr_acc.balance.amount == value.amount ) {
			accounts.erase(sndr_acc);
		} else {
			accounts.modify(sndr_acc, payer, [&](auto& a) {
				a.balance -= value;
			});
		}
	}

	void SmtsBusiness::InvRequest(const name& from, const name& to, const asset& quantity) {

		require_auth(from);

		CommodityUnitPrices commUnitPrices(_self, to.value);
		const auto& existing_comm_prcs_iter = commUnitPrices.get(quantity.symbol.code().raw(),
			"Make sure a Commodity (Scrap Metal) exists before placing an order");

		InventoryReqs invRequests(_self, _self.value);

		invRequests.emplace(from, [&](auto& req) {
			req.id         = invRequests.available_primary_key();
			req.from       = from;
			req.to         = to;
			req.unit_price = existing_comm_prcs_iter.unit_price;
			req.quantity   = quantity;
			req.status     = "PENDING";
		});
	}

	void SmtsBusiness::Fulfill(const name& merchant_acct, uint64_t ir_id) {

		require_auth(merchant_acct);

		InventoryReqs invRequests(_self, _self.value);
		const auto& inv_req = invRequests.get(ir_id, "Fulfill Requests that exist only");

		// An inventory request is fulfilled by transfering some specified amount of Tokens from 
		// one acct to another
		action {
			permission_level{inv_req.to, "active"_n},
			_self,
			"transfer"_n,
			std::make_tuple(inv_req.to, inv_req.from, inv_req.quantity, std::string{"Inv Req fulfilled"})
		}.send();

		invRequests.modify(inv_req, merchant_acct, [&](auto& req) {
			req.status = "FULFILLED";
		});
	}

	void SmtsBusiness::Pay(const name& customerAcct, uint64_t irId) {

		require_auth(customerAcct);
		InventoryReqs invRequests(_self, _self.value);
		const auto& inv_req = invRequests.get(irId, "Settle Requests that exist only");

		eosio_assert(inv_req.from == customerAcct, "You cannot Settle a request that you didn't initiate");
		eosio_assert(inv_req.status == "FULFILLED", "Settling a Req that's not at the 'FULFILLED' stage is not allowed");

		// make the transfer
		action {
			permission_level{customerAcct, "active"_n},
			_self,
			"transfer"_n,
			std::make_tuple(
				customerAcct, inv_req.to, (inv_req.unit_price * inv_req.quantity.amount), 
				std::string{"Inv Req settled"})
		}.send();

		invRequests.modify(inv_req, customerAcct, [&](auto& req) {
			req.status = "PAID";
		});
	}

	void SmtsBusiness::DeleteData() {

		require_auth(_self);
		asset yelos = {0, {"YELOS", 4}};
		asset iron = {0, {"IRON", 2}};

		Stats st1(_self, yelos.symbol.code().raw());
		auto itr1 = st1.begin();

		while(itr1 != st1.end()){
			itr1 = st1.erase(itr1);
		}

		Stats ironSt(_self, iron.symbol.code().raw());
		auto ironStitr = ironSt.begin();

		while(ironStitr != ironSt.end()){
			ironStitr = ironSt.erase(ironStitr);
		}

		Accounts st(_self, ("eosyelosbobb"_n).value);
		auto itr = st.begin();

		while(itr != st.end()){
			itr = st.erase(itr);
		}

		Accounts st2(_self, ("eosyeloserik"_n).value);
		auto itr2 = st2.begin();

		while(itr2 != st2.end()){
			itr2 = st2.erase(itr2);
		}

		CommodityUnitPrices commUnitPrices(_self, "eosyeloserik"_n.value);
		auto itr3 = commUnitPrices.begin();

		while(itr3 != commUnitPrices.end()){
			itr3 = commUnitPrices.erase(itr3);
		}

		InventoryReqs invRequests(_self, _self.value);
		auto iter4 = invRequests.begin();

		while(iter4 != invRequests.end()) {
			iter4 = invRequests.erase(iter4);
		}
	}
}