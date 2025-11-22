#include "mud/core/uri.h"
#include "mud/test.h"
#include <sstream>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() = default;

    /* Destructor after each scenario */
    ~context() = default;

    /* The uri */
    mud::core::uri uri;

    /* A string */
    std::string str;
END_CONTEXT()

FEATURE("URI")
    DEFINE_GIVEN("An empty URI", [](context& ctx){
    })
    DEFINE_GIVEN("An HTTP URI", [](context& ctx){
      ctx.uri.scheme("http");
      ctx.uri.user_info("admin");
      ctx.uri.host("example.com");
      ctx.uri.port(443);
      ctx.uri.path("some/place/here/index.html");
      ctx.uri.query("key1=value1&key2=value2");
      ctx.uri.fragment("top");
    })
    DEFINE_WHEN("The URI is examined", [](context& ctx){
    })
    DEFINE_WHEN("The URI is normalised", [](context& ctx){
      ctx.uri.normalise();
    })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("URI type traits")
    GIVEN("A uri type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::uri>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::core::uri>::value);
        })
    AND  ("The type is  copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::core::uri>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::uri>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::uri>::value);
        })

  SCENARIO("Generic URI fields can be set and retrieved")
    GIVEN("An HTTP URI")
    WHEN ("The URI is examined")
    THEN ("The fields are set",
        [](context& ctx){
          ASSERT("http", ctx.uri.scheme());
          ASSERT("admin", ctx.uri.user_info());
          ASSERT("example.com", ctx.uri.host());
          ASSERT(443, ctx.uri.port());
          ASSERT("admin@example.com:443", ctx.uri.authority());
          ASSERT("some/place/here/index.html", ctx.uri.path());
          ASSERT("key1=value1&key2=value2", ctx.uri.query());
          ASSERT("top", ctx.uri.fragment());
        })

  SCENARIO("A Generic URI can be parsed into fields")
    GIVEN("An HTTP URI",
        [](context& ctx){
            ctx.uri = mud::core::uri("http://admin@example.com:443/some/place/here/index.html?key1=value1&key2=value2#top");
        })
    WHEN ("The URI is examined")
    THEN ("The fields are set",
        [](context& ctx){
          ASSERT("http", ctx.uri.scheme());
          ASSERT("admin", ctx.uri.user_info());
          ASSERT("example.com", ctx.uri.host());
          ASSERT(443, ctx.uri.port());
          ASSERT("admin@example.com:443", ctx.uri.authority());
          ASSERT("/some/place/here/index.html", ctx.uri.path());
          ASSERT("key1=value1&key2=value2", ctx.uri.query());
          ASSERT("top", ctx.uri.fragment());
          ASSERT(2, ctx.uri.query_params().size());
          auto iter = ctx.uri.query_params().begin();
          ASSERT("key1", iter->first);
          ASSERT("value1", iter->second);
          ++iter;
          ASSERT("key2", iter->first);
          ASSERT("value2", iter->second);
        })

  SCENARIO("The scheme is case-insensitive")
    GIVEN("An empty URI")
    WHEN ("A scheme is set with mixed capitalisation",
        [](context& ctx) {
          ctx.uri.scheme("HtTp");
        })
    THEN ("The scheme is changed to lower-case",
        [](context& ctx) {
          ASSERT("http", ctx.uri.scheme());
        })

  SCENARIO("The user-info is case-sensitive")
    GIVEN("An empty URI")
    WHEN ("A user-info is set with capitalsation",
        [](context& ctx) {
          ctx.uri.user_info("John.Doe");
        })
    THEN ("The user-info is unchanged",
        [](context& ctx) {
          ASSERT("John.Doe", ctx.uri.user_info());
        })

  SCENARIO("The host is case-insensitive")
    GIVEN("An empty URI")
    WHEN ("A host is set with capitalsation",
        [](context& ctx) {
          ctx.uri.host("ExAmPlE.CoM");
        })
    THEN ("The user-info is unchanged",
        [](context& ctx) {
          ASSERT("example.com", ctx.uri.host());
        })

  SCENARIO("The path is case-sensitive")
    GIVEN("An empty URI")
    WHEN ("A path is set with capitalsation",
        [](context& ctx) {
          ctx.uri.path("/SoMe/WhErE");
        })
    THEN ("The path is unchanged",
        [](context& ctx) {
          ASSERT("/SoMe/WhErE", ctx.uri.path());
        })

  SCENARIO("The path is normalised")
    GIVEN("An empty URI")
    WHEN ("A path is set as <unnormalised>",
        [](context& ctx) {
          ctx.uri.path(ctx.sample<std::string>("unnormalised"));
        })
    THEN ("The path is <normalised>",
        [](context& ctx) {
          ASSERT(ctx.sample<std::string>("normalised"), ctx.uri.path());
        })
    SAMPLES(std::string, std::string)
        HEADINGS("unnormalised", "normalised")
        SAMPLE("hello/../dear/world", "dear/world")
        SAMPLE("hello/./dear/world", "hello/dear/world")
        SAMPLE("hello/../world/..", ".")
        SAMPLE("hello/../world/../..", "..")
        SAMPLE("hello/../world/../../..", "../..")
    END_SAMPLES()

  SCENARIO("The query is case-sensitive")
    GIVEN("An empty URI")
    WHEN ("A query is set with capitalsation",
        [](context& ctx) {
          ctx.uri.query("Id=ObJeCt");
        })
    THEN ("The query is unchanged",
        [](context& ctx) {
          ASSERT("Id=ObJeCt", ctx.uri.query());
        })

  SCENARIO("The fragment is case-sensitive")
    GIVEN("An empty URI")
    WHEN ("A fragment is set with capitalsation",
        [](context& ctx) {
          ctx.uri.fragment("Top");
        })
    THEN ("The fragment is unchanged",
        [](context& ctx) {
          ASSERT("Top", ctx.uri.fragment());
        })

  SCENARIO("The generic URI query can be decoded")
    GIVEN("An empty URI")
    WHEN ("A URI is parsed that needs decoding",
        [](context& ctx) {
          ctx.uri = mud::core::uri(
              "http://example.com/index.html?id=10&name=Name%3DHello%26World?");
        })
    THEN ("The URI's query is decoded",
        [](context& ctx) {
          ASSERT("id=10&name=Name=Hello&World?", ctx.uri.query());
          ASSERT(2, ctx.uri.query_params().size());
          auto iter = ctx.uri.query_params().begin();
          ASSERT("id", iter->first);
          ASSERT("10", iter->second);
          ++iter;
          ASSERT("name", iter->first);
          ASSERT("Name=Hello&World?", iter->second);
        })

  SCENARIO("The generic URI query can be encoded")
    GIVEN("An empty URI")
    WHEN ("An URI query that needs encoding",
        [](context& ctx) {
          ctx.uri.scheme("http");
          ctx.uri.host("example.com");
          ctx.uri.path("index.html");
          mud::core::uri::query_params_t params;
          params.push_back(std::make_pair("id", "10"));
          params.push_back(std::make_pair("name", "Name=Hello&World?"));
          ctx.uri.query_params(params);
        })
    THEN ("A URI is encoded",
        [](context& ctx) {
          std::stringstream sstr;
          sstr << ctx.uri;
          ASSERT("http://example.com/index.html?id=10&name=Name%3DHello%26World%3F", sstr.str());
        })

  SCENARIO("A relative URI can be created")
    GIVEN("An empty URI")
    WHEN ("A relative URI is configured",
        [](context& ctx) {
          ctx.uri.path("/www/index.html");
          ctx.uri.query("id=10");
          ctx.uri.fragment("top");
        })
    THEN ("The URI can be created",
        [](context& ctx) {
          std::stringstream sstr;
          sstr << ctx.uri;
          ASSERT("/www/index.html?id=10#top", sstr.str());
        })

  SCENARIO("A relative URI can be parsed")
    GIVEN("A relative URI",
        [](context& ctx) {
          ctx.uri = mud::core::uri("/www/index.html?id=10#top");
        })
    WHEN ("The URI is examined")
    THEN ("A relative can be qeuried",
        [](context& ctx) {
          ASSERT("/www/index.html", ctx.uri.path());
          ASSERT("id=10", ctx.uri.query());
          ASSERT("top", ctx.uri.fragment());
        })

  SCENARIO("A HTTP URI can be created")
    GIVEN("An empty URI")
    WHEN ("An HTTP URI is configured",
        [](context& ctx) {
          ctx.uri.scheme("http");
          ctx.uri.host("example.com");
          ctx.uri.path("www/index.html");
          ctx.uri.query("id=10");
          ctx.uri.fragment("top");
        })
    THEN ("The URI can be created",
        [](context& ctx) {
          std::stringstream sstr;
          sstr << ctx.uri;
          ASSERT("http://example.com/www/index.html?id=10#top", sstr.str());
        })

  SCENARIO("A normalised HTTP URI has default values")
    GIVEN("An simple HTTP",
        [](context& ctx) {
          ctx.uri.scheme("http");
          ctx.uri.host("example.com");
        })
    WHEN ("The URI is normalised")
    THEN ("The components are normalised per HTTP specification",
        [](context& ctx) {
          ASSERT(80, ctx.uri.port());
          ASSERT("/", ctx.uri.path());
        })

  SCENARIO("Various URI formats can be parsed")
    GIVEN("A '<uri>' URI as a string", [](context& ctx) {
          ctx.str = ctx.sample<std::string>("uri");
        })
    WHEN ("The URI is parsed", [](context& ctx) {
          ctx.uri = mud::core::uri(ctx.str);
        })
    THEN ("The URI is correctly formed", [](context& ctx) {
          ASSERT(ctx.sample<std::string>("scheme"),
                 ctx.uri.scheme());
          ASSERT(ctx.sample<std::string>("userinfo"),
                 ctx.uri.user_info());
          ASSERT(ctx.sample<std::string>("host"),
                 ctx.uri.host());
          ASSERT(ctx.sample<uint16_t>("port"),
                 ctx.uri.port());
          ASSERT(ctx.sample<std::string>("path"),
                 ctx.uri.path());
          ASSERT(ctx.sample<std::string>("query"),
                 ctx.uri.query());
          ASSERT(ctx.sample<std::string>("fragment"),
                 ctx.uri.fragment());
        })
    SAMPLES(std::string, std::string, std::string, std::string, uint16_t, std::string, std::string, std::string)
        HEADINGS("uri", "scheme", "userinfo", "host", "port", "path", "query","fragment")
        SAMPLE("http://www.example.com/index.html",                                "http",   "",         "www.example.com", 80,     "/index.html",      "",                       "")
        SAMPLE("http://www.example.com/index.html?id=10",                          "http",   "",         "www.example.com", 80,     "/index.html",      "id=10",                  "")
        SAMPLE("http://www.example.com/index.html?id=10#top",                      "http",   "",         "www.example.com", 80,     "/index.html",      "id=10",                  "top")
        SAMPLE("https://www.example.com:8080/index.html?id=10&name=Hello%20World", "https",  "",         "www.example.com", 8080,   "/index.html",      "id=10&name=Hello World", "")
        SAMPLE("ftp://www.example.com/dir/a.out",                                  "ftp",    "",         "www.example.com", 21,     "/dir/a.out",       "",                       "")
        SAMPLE("/etc/sample.conf",                                                 "",       "",         "",                0,      "/etc/sample.conf", "",                       "")
        SAMPLE("sample.conf",                                                      "",       "",         "",                0,      "sample.conf",      "",                       "")
        SAMPLE("./sample.conf",                                                    "",       "",         "",                0,      "sample.conf",      "",                       "")
        SAMPLE("../sample.conf",                                                   "",       "",         "",                0,      "../sample.conf",   "",                       "")
        SAMPLE("*",                                                                "",       "",         "",                0,      "*",                "",                       "")
    END_SAMPLES()

  SCENARIO("Various URI formats can be created")
    GIVEN("An empty URI")
    WHEN ("The URI is configured", [](context& ctx) {
          ctx.uri.scheme(ctx.sample<std::string>("scheme"));
          ctx.uri.user_info(ctx.sample<std::string>("userinfo"));
          ctx.uri.host(ctx.sample<std::string>("host"));
          ctx.uri.port(ctx.sample<uint16_t>("port"));
          ctx.uri.path(ctx.sample<std::string>("path"));
          ctx.uri.query(ctx.sample<std::string>("query"));
          ctx.uri.fragment(ctx.sample<std::string>("fragment"));
        })
    THEN ("The URI is correctly formed", [](context& ctx) {
          std::stringstream sstr;
          sstr << ctx.uri;
          ASSERT(ctx.sample<std::string>("uri"), sstr.str());
        })
    SAMPLES(std::string, std::string, std::string, uint16_t, std::string, std::string, std::string, std::string)
        HEADINGS("scheme", "userinfo", "host", "port", "path", "query", "fragment", "uri")
        SAMPLE("http",   "",         "www.example.com", 80,     "/index.html",      "",                         "",         "http://www.example.com/index.html")
        SAMPLE("http",   "",         "www.example.com", 80,     "/index.html",      "id=10",                    "",         "http://www.example.com/index.html?id=10")
        SAMPLE("http",   "",         "www.example.com", 80,     "/index.html",      "id=10",                    "top",      "http://www.example.com/index.html?id=10#top")
        SAMPLE("https",  "",         "www.example.com", 8080,   "/index.html",      "id=10&name=Hello%20World", "",         "https://www.example.com:8080/index.html?id=10&name=Hello%20World")
        SAMPLE("ftp",    "",         "www.example.com", 21,     "/dir/a.out",       "",                         "",         "ftp://www.example.com/dir/a.out")
        SAMPLE("",       "",         "",                0,      "/etc/sample.conf", "",                         "",         "/etc/sample.conf")
        SAMPLE("",       "",         "",                0,      "sample.conf",      "",                         "",         "sample.conf")
        SAMPLE("",       "",         "",                0,      "./sample.conf",    "",                         "",         "sample.conf")
        SAMPLE("",       "",         "",                0,      "../sample.conf",   "",                         "",         "../sample.conf")
        SAMPLE("",       "",         "",                0,      "*",                "",                         "",         "*")
    END_SAMPLES()

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
