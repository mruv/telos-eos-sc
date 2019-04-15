
#include "smtsbusiness.hpp"

namespace SmtsBusiness {

	void SmtsBusiness::CreateToken(const asset& max_supply) {

		// Currency assets can only be created by the owner of this contract
		require_auth(_self);

		asset yelos = {0, symbol{"YELOS", 4}};
		eosio_assert(yelos.symbol.code().raw() == max_supply.symbol.code().raw(), "YELOS is the contract's only currency / token");
		Create(_self, max_supply);
	}

	void SmtsBusiness::CreateCmmdty(const name& issuer, const asset& max_supply, const asset& unit_price) {

		require_auth(issuer);

		// Accounts curr_acc(_self, issuer);
		Stats commodityStats(_self, unit_price.symbol.code().raw());
		auto existing_stats_iter = commodityStats.find(unit_price.symbol.code().raw());
		eosio_assert(existing_stats_iter != commodityStats.end(), "specified currency doesn't exist");

		// auto existing = statsdata.find(asset_sym.code().raw());
		Create(issuer, max_supply);
		CommodityUnitPrices commUnitPrices(_self, issuer.value);
		auto existing_smrts_iter = commUnitPrices.find(max_supply.symbol.code().raw());

		if (existing_smrts_iter == commUnitPrices.end()) {
			// create new
			commUnitPrices.emplace(issuer, [&](auto& smrate) {
				smrate.comm_name = max_supply.symbol.code();
				smrate.unit_price = unit_price;
			});
		} else {
			// update
			commUnitPrices.modify(existing_smrts_iter, issuer, [&](auto& smrate) {
				smrate.unit_price = unit_price;
			});
		}
	}

	// Define new configuration for a new asset
	void SmtsBusiness::Create(const name& issuer, const asset& max_supply) {

		auto asset_sym = max_supply.symbol;
		eosio_assert(asset_sym.is_valid(), "invalid symbol name" );
		eosio_assert(max_supply.is_valid(), "invalid supply");
		eosio_assert(max_supply.amount > 0, "max-supply must be positive");

		Stats assetStats(_self, asset_sym.code().raw());
		auto existing = assetStats.find(asset_sym.code().raw());
		eosio_assert(existing == assetStats.end(), "token with symbol already exists");

		// create a new asset
		assetStats.emplace(_self, [&](auto& s ) {
		   s.supply.symbol = max_supply.symbol;
		   s.max_supply    = max_supply;
		   s.issuer        = issuer;
		});
	}

	void SmtsBusiness::Issue(const name& to, const asset& quantity, const std::string& memo) {

		auto asset_sym = quantity.symbol;
	    eosio_assert(asset_sym.is_valid(), "invalid symbol name");
	    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

	    // auto asset_sym_val = asset_sym.raw();
	    // code --> _self --> contract
	    // scope --> _self.value --> contract
	    Stats tokenStats(_self, asset_sym.code().raw());
	    auto existing = tokenStats.find(asset_sym.code().raw());
	    eosio_assert(existing != tokenStats.end(), "asset with symbol does not exist, create token before Issue");

	    // An asset can only be Issued by the creator.
	    require_auth(existing->issuer);
	    eosio_assert(quantity.is_valid(), "invalid quantity" );
	    eosio_assert(quantity.amount > 0, "must Issue positive quantity" );

	    eosio_assert(quantity.symbol == existing->supply.symbol, "symbol precision mismatch");
	    eosio_assert(quantity.amount <= existing->max_supply.amount - existing->supply.amount, "quantity exceeds available supply");

	    tokenStats.modify(*existing, existing->issuer, [&]( auto& s) {
	       s.supply += quantity;
	    });

	    UpdateDestAcct(existing->issuer, existing->issuer, quantity);

	    if(to != existing->issuer ) {
	    	action inline_action = action(
	    		permission_level{existing->issuer,"active"_n},
	    		// account that owns the contract. In this case, the name of the contract == name of the account
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
			// this only happens when the account 'to' is receiving the asset for the
			// first time.
			// to == from
			accounts.emplace(payer, [&]( auto& a ) {
				a.balance = value;
			});
		} else {
			accounts.modify(rcvr_acc, payer, [&]( auto& a ) {
				a.balance += value;
			});
		}
	}

	void SmtsBusiness::UpdateSrcAcct(const name& payer, const asset& value) {

		Accounts accounts(_self, payer.value);
		const auto& sndr_acc = accounts.get(value.symbol.raw(), "no balance object found");
		eosio_assert(sndr_acc.balance.amount >= value.amount, "overdrawn balance" );

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
			req.status     = IrStatus::PENDING;
		});
	}

	void SmtsBusiness::Fulfill(const name& merchant_acct, uint64_t ir_id) {
		
	}

	void SmtsBusiness::Pay(const name& customer_acct, uint64_t ir_id) {

	}

	void SmtsBusiness::DeleteData() {

		require_auth(_self);
		asset yelos = {0, {"YELOS", 4}};

		Stats st1(_self, yelos.symbol.code().raw());
		auto itr1 = st1.begin();

		while(itr1 != st1.end()){
			itr1 = st1.erase(itr1);
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
	}
}