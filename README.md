lousy-leveldb
=============

Lousy Leveldb is leveldb with "lousy" features. Currently it supports delta updates. The delta updates are merged in the `Get()` operation and during background compaction gradually.

Please note this code has not yet been thoroughly tested.

Usage
-----

When opening a db, you need to specify a Value Merger, in which two methods need to be implemented.

    virtual int Merge(std::string *value, bool partial, const Slice& new_value) const = 0;
    virtual bool Shrink(std::string *value) const = 0;

For a sample of Value Merger, take a look at `db/db_test.cc`. Below implements a concatenate value merger (deletion starts with '-').

		class ConcatValueMerger: public ValueMerger {
		private:
			std::string delimiter_;

		private:
			static void parse_item(const std::string& value, size_t start, size_t end, std::set<std::string>& items) {
				bool is_negative = (value[start] == '-');
				if (is_negative) start++;
				std::string item = (end == std::string::npos)? value.substr(start): value.substr(start, end - start);
				if (item.empty()) return;
				if (is_negative) items.erase(item);
				else items.insert(item);
			}

		public:
			explicit ConcatValueMerger(const char *delimiter) : delimiter_(delimiter) {}
			int Merge(std::string *value, bool partial, const Slice& new_value) const {
				if (!value->empty() && !delimiter_.empty()) value->append(delimiter_);
				value->append(new_value.data(), new_value.size());
				return 0;
			}

			bool Shrink(std::string *value) const {
				size_t start = std::string::npos, end = std::string::npos;
				std::set<std::string> items;
				while ( (start = value->rfind(delimiter_, end)) != std::string::npos) {
					parse_item(*value, start + delimiter_.length(), end == std::string::npos? end: end + 1, items);
					if (start == 0) break;
					end = start - 1;
				}
				if (start != 0) parse_item(*value, 0, end == std::string::npos? end: end + 1, items);

				std::string s;
				for(std::set<std::string>::const_iterator it = items.begin(); it != items.end(); ++it) {
					s += (*it);
					s += ",";
				}
				if (!s.empty()) s.resize(s.length() - 1);
				value->swap(s);

				return true;
			}

			virtual const char* Name() const { return "leveldb.ConcatValueMerger"; }
		};

To specify a value merger when opening a DB:

		ConcatValueMerger cvm(",");
		options.merger  = &cvm;

To insert delta updates, use `Append()`. And `Get()` will return the whole record.


