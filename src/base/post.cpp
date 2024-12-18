#include "config.hpp"

#include "fibjs.h"
#include "object.h"
#include "Buffer.h"
#include "ifs/chain.h"
#include "ifs/http.h"

#include <eosio/chain/application.hpp>
#include <fc/exception/exception.hpp>
#include <fc/reflect/reflect.hpp>
#include <fc/io/json.hpp>
#include <fc/reflect/variant.hpp>

#define private public
#include <eosio/http_plugin/http_plugin.hpp>
#undef private

#define make_http_response_handler post_make_http_response_handler
#define host_port_is_valid post_host_port_is_valid
#define host_is_valid post_host_is_valid
#include <eosio/http_plugin/common.hpp>

#undef make_http_response_handler
#undef host_port_is_valid
#undef host_is_valid

namespace eosio {
namespace detail {
    class derived_conn : public eosio::detail::abstract_conn {
        std::string verify_max_bytes_in_flight(size_t extra_bytes) override
        {
            return {};
        }

        std::string verify_max_requests_in_flight() override
        {
            return {};
        }

        void send_busy_response(std::string&& what) override { }

        void handle_exception() override { }

        void send_response(std::string&& json_body, unsigned int code) override { }
    };
}

class http_plugin_impl : public std::enable_shared_from_this<http_plugin_impl> {
public:
    std::map<std::string, api_category_set> categories_by_address;
    std::shared_ptr<http_plugin_state> plugin_state;
};

}

using namespace eosio;

namespace fibjs {

result_t chain_base::post(exlib::string resource, exlib::string body, exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    app().executor().post(appbase::priority::low, exec_queue::read_only, [resource { std::move(resource) }, body { std::move(body) }, ac, &retVal]() {
        auto url_handlers = app().get_plugin<http_plugin>().my->plugin_state->url_handlers;
        auto handler_itr = url_handlers.find(resource);

        if (handler_itr == url_handlers.end()) {
            ac->post(CALL_E_INVALIDARG);
        } else {
            detail::internal_url_handler_fn* hdlr = new detail::internal_url_handler_fn(handler_itr->second.fn);
            auto _conn = std::make_shared<detail::derived_conn>();
            try {
                (*hdlr)(_conn, std::move(resource), std::move(body),
                    [=, &retVal](int code, std::optional<fc::variant> result) {
                        std::string json = fc::json::to_string(result, fc::time_point::maximum());
                        retVal = json;
                        ac->post(0);
                        delete hdlr;
                    });
            } catch (...) {
                ac->post(CALL_E_INTERNAL);
                delete hdlr;
            }
        }
    });

    return CALL_E_PENDDING;
}
} // namespace fibjs
