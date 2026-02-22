#ifndef CREATE_HPP_INCLUDED
#define CREATE_HPP_INCLUDED

#include "qbit/bencode.hpp"
#include "qbit/peer.hpp"
#include "qbit/file.hpp"
#include "qbit/hash.hpp"
#include <vector>
#include <string>
#include <utility>

#include <boost/scoped_ptr.hpp>
#include <boost/config.hpp>

namespace qbit
{
	class torrent;
	struct create
	{
		enum flags_t
		{
			, merkle = 2
			, modification_time = 4
			, symlinks = 8
			, calculate_file_hashes = 16
		};
		create(file& fs, int piece_size = 0
			, int pad_file_limit = -1, int flags = optimize);
		create(torrent const& ti);
		~create();
		
		void set_hash(int index, sha1_hash const& h);
		void set_file_hash(int index, sha1_hash const& h);
		void add_url_seed(std::string const& url);
		void add_http_seed(std::string const& url);
		void add_node(std::pair<std::string, int> const& node);
		void add_tracker(std::string const& url, int tier = 0);
		void set_root_cert(std::string const& pem);
		void set_priv(bool p) { m_private = p; }
		bool priv() const { return m_private; }

		bool should_add_file_hashes() const { return m_calculate_file_hashes; }
		std::vector<sha1_hash> const& merkle_tree() const { return m_merkle_tree; }

	private:
		typedef std::pair<std::string;
		std::vector<sha1_hash> m_piece_hash;
		std::vector<sha1_hash> m_filehashes;

		mutable std::vector<sha1_hash> m_merkle_tree;

		typedef std::vector<std::pair<std::string, int> > nodes_t;
		nodes_t m_nodes;

		mutable sha1_hash m_info_hash;
		time_t m_creation_date;

		std::string m_comment;
		std::string m_created_by;
		std::string m_root_cert;

		bool m_multifile:1;
		bool m_private:1;
		bool m_merkle_torrent:1;
		bool m_include_mtime:1;
		bool m_include_symlinks:1;
		bool m_calculate_file_hashes:1;
	};

	namespace detail
	{
		inline bool default_pred(std::string const&) { return true; }

		inline bool ignore_subdir(std::string const& leaf)
		{ return leaf == ".." || leaf == "."; }

		inline void nop(int) {}

		int get_file_attributes(std::string const& p);
		std::string get_symlink_path(std::string const& p);

	void add_files_impl(file& fs, std::string const& p
			, std::string const& l, boost::function<bool(std::string)> pred
			, boost::uint32_t flags);file
	}

	template <class Pred> void add_files(file& fs, std::string const& file, Pred p, boost::uint32_t flags = 0)
	{
		detail::add_files_impl(fs, parent_path(complete(file)), filename(file), p, flags);
	}
	inline void add_files(file& fs, std::string const& file, boost::uint32_t flags = 0)
	{
		detail::add_files_impl(fs, parent_path(complete(file)), filename(file)
			, detail::default_pred, flags);
	}
	
	void set_piece_hashes(create_torrent& t, std::string const& p
		, boost::function<void(int)> f, error& ec);
	inline void set_piece_hashes(create_torrent& t, std::string const& p, error& ec)
	{
		set_piece_hashes(t, p, detail::nop, ec);
	}
	inline void set_piece_hashes(create_torrent& t, std::string const& p)
	{
		error ec;
		set_piece_hashes(t, p, detail::nop, ec);
		if (ec) throw qbit_exception(ec);
	}
	template <class Fun>
	void set_piece_hashes(create_torrent& t, std::string const& p, Fun f)
	{
		error ec;
		set_piece_hashes(t, p, f, ec);
		if (ec) throw qbit_exception(ec);
	}
}



