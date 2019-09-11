/**
 * The Forgotten Server - a free and open-source MMORPG server emulator
 * Copyright (C) 2019  Mark Samman <mark.samman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef FS_IOLOGINDATA_H_28B0440BEC594654AC0F4E1A5E42B2EF
#define FS_IOLOGINDATA_H_28B0440BEC594654AC0F4E1A5E42B2EF

#include "account.h"
#include "player.h"
#include "database.h"
#include "inbox.h"

using ItemBlockList = std::list<std::pair<int32_t, Item*>>;

class IOLoginData
{
	public:
		static Account loadAccount(uint32_t accno);
		static bool saveAccount(const Account& acc);

		static bool loginserverAuthentication(const std::string& name, const std::string& password, Account& account);
		static uint32_t gameworldAuthentication(const std::string& accountName, const std::string& password, std::string& characterName, std::string& token, uint32_t tokenTime);

		static AccountType_t getAccountType(uint32_t accountId);
		static void setAccountType(uint32_t accountId, AccountType_t accountType);
		static void updateOnlineStatus(uint32_t guid, bool login);
		static bool preloadPlayer(Player* player, const std::string& name);

		static bool loadPlayerById(Player* player, uint32_t id);
		static bool loadPlayerByName(Player* player, const std::string& name);
		static bool loadPlayer(Player* player, DBResult_ptr result);
		static bool savePlayer(Player* player);
		static uint32_t getGuidByName(const std::string& name);
		static bool getGuidByNameEx(uint32_t& guid, bool& specialVip, std::string& name);
		static std::string getNameByGuid(uint32_t guid);
		static bool formatPlayerName(std::string& name);
		static void increaseBankBalance(uint32_t guid, uint64_t bankBalance);
		static bool hasBiddedOnHouse(uint32_t guid);

		static std::forward_list<VIPEntry> getVIPEntries(uint32_t accountId);
		static void addVIPEntry(uint32_t accountId, uint32_t guid, const std::string& description, uint32_t icon, bool notify);
		static void editVIPEntry(uint32_t accountId, uint32_t guid, const std::string& description, uint32_t icon, bool notify);
		static void removeVIPEntry(uint32_t accountId, uint32_t guid);

		static void addPremiumDays(uint32_t accountId, int32_t addDays);
		static void removePremiumDays(uint32_t accountId, int32_t removeDays);

		static bool savePlayerItems(uint32_t guid, Item** inventory, std::map<uint32_t, DepotChest*>& depotChests, Inbox* inbox, int16_t lastDepotId, Database* database = nullptr);

		static bool savePlayerBinaryItems(uint32_t guid, Item** inventory, std::map<uint32_t, DepotChest*>& depotChests, Inbox* inbox, int16_t lastDepotId, Database* db);

	private:
		using ItemMap = std::map<uint32_t, std::pair<Item*, uint32_t>>;

		static void loadPlayerItems(Player* player, Database* db);
		static void loadItems(ItemMap& itemMap, DBResult_ptr result);
		static bool saveItems(uint32_t guid, const ItemBlockList& itemList, DBInsert& query_insert, PropWriteStream& propWriteStream);

		static void loadPlayerBinaryItems(Player* player, Database* db);
		static Item* loadBinaryItem(PropStream& itemsStream, Container* parent);
		static bool saveBinaryItem(Item*item, PropWriteStream& itemsStream);
};

class PlayerCacheData;

class PlayerCacheManager : public ThreadHolder<PlayerCacheManager>
{
	public:
		PlayerCacheManager() = default;

		bool loadCachedPlayer(uint32_t guid, Player* player);
		void cachePlayer(uint32_t guid, Player* player);
		void clear();
		void clear(uint32_t guid);

		void start();
		void flush();
		void shutdown();

		void addToSaveList(uint32_t guid);
		bool saveCachedItems(uint32_t guid);

		void threadMain();

	private:
		PlayerCacheData* getCachedPlayer(uint32_t guid, bool autoCreate = false);

		Database db;
		std::thread thread;
		std::map<uint32_t, int64_t> toSave;
		std::mutex listLock;

		std::map<uint32_t, PlayerCacheData*> playersCache;
};

class PlayerCacheData
{
	public:
		~PlayerCacheData();
		PlayerCacheData* clone();
		void copyDataFromPlayer(Player* player);
		void copyDataToPlayer(Player* player);

	private:
		void clear();

		Item* inventory[CONST_SLOT_LAST + 1] = {};
		std::map<uint32_t, DepotChest*> depotChests;
		Inbox* inbox = nullptr;
		int16_t lastDepotId = -1;
		std::mutex dataLock;

		friend class PlayerCacheManager;
};

#endif
