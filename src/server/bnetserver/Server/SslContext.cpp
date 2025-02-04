/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "SslContext.h"
#include "Config.h"
#include "Log.h"

bool Battlenet::SslContext::Initialize()
{
    boost::system::error_code err;

#define LOAD_CHECK(fn) do { fn; \
    if (err) \
    { \
        TC_LOG_ERROR("server.ssl", #fn " failed: {}", err.message()); \
        return false; \
    } } while (0)

    std::string certificateChainFile = sConfigMgr->GetStringDefault("CertificatesFile", "./bnetserver.cert.pem");
    std::string privateKeyFile = sConfigMgr->GetStringDefault("PrivateKeyFile", "./bnetserver.key.pem");

    auto passwordCallback = [](std::size_t /*max_length*/, boost::asio::ssl::context::password_purpose /*purpose*/) -> std::string
    {
        return sConfigMgr->GetStringDefault("PrivateKeyPassword", "");
    };

    LOAD_CHECK(instance().set_password_callback(passwordCallback, err));
    LOAD_CHECK(instance().use_certificate_chain_file(certificateChainFile, err));
    LOAD_CHECK(instance().use_private_key_file(privateKeyFile, boost::asio::ssl::context::pem, err));

#undef LOAD_CHECK

    return true;
}

boost::asio::ssl::context& Battlenet::SslContext::instance()
{
    static boost::asio::ssl::context context(boost::asio::ssl::context::tls);
    return context;
}
