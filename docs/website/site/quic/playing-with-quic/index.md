---
breadcrumbs:
- - /quic
  - QUIC, a multiplexed transport over UDP
page_name: playing-with-quic
title: Playing with QUIC
---

### **Build the QUIC client and server**

A sample server and client implementation are provided in Chromium. To use these
you should first have [checked out the Chromium
source](/developers/how-tos/get-the-code), and then build the binaries:

```none
ninja -C out/Debug quic_server quic_client
```

### **Prep test data from www.example.org**

Download a copy of www.example.org, which we will serve locally using the
quic_server binary:

```none
mkdir /tmp/quic-data
cd /tmp/quic-data
wget -p --save-headers https://www.example.org
```

Manually edit index.html and adjust the headers:

    *   **Remove (if it exists):** "Transfer-Encoding: chunked"
    *   **Remove (if it exists):** "Alternate-Protocol: ..."
    *   **Add:** X-Original-Url: https://www.example.org/

### Generate certificates

In order to run the server, you will need a valid certificate, and a private key
in pkcs8 format. If you don't have one, there are scripts you can use to
generate them:

```none
cd net/tools/quic/certs
./generate-certs.sh
cd -
```

In addition to the server's certificate and public key, this script will also
generate a CA certificate (`net/tools/quic/certs/out/2048-sha256-root.pem`)
which you will need to add to your OS's root certificate store in order for it
to be trusted during certificate validation. For doing this on Linux, please see
[these
instructions](https://chromium.googlesource.com/chromium/src/+/HEAD/docs/linux/cert_management.md).
This will allow quic_client to verify the certificate correctly. However, note
that Chrome/Chromium (the browser) does not allow custom CAs for QUIC, so you'll
also need to pass in --ignore-certificate-errors-spki-list with the
certificate's spki to allow Chrome/Chromium to accept your custom certificate as
valid.

### Run the QUIC server and client

Run the quic_server:

```none
./out/Debug/quic_server \
  --quic_response_cache_dir=/tmp/quic-data/www.example.org \
  --certificate_file=net/tools/quic/certs/out/leaf_cert.pem \
  --key_file=net/tools/quic/certs/out/leaf_cert.pkcs8
```

And you should be able to successfully request the file over QUIC using
quic_client:

```none
./out/Debug/quic_client --host=127.0.0.1 --port=6121 https://www.example.org/
```

Note that if you let the server's port default to 6121, you must specify the
client port because it defaults to 80.

Moreover, if your local machine has multiple loopback addresses (as it would if
using both IPv4 and IPv6), you have to pick a specific address.

It remains to be determined whether the latter shortcoming is a bug.

If the server you are connecting to does not have a trusted certificate, use the
`--disable_certificate_verification` flag on the client to disable certificate
verification. If the server's certificate is trusted but chains to a user
installed CA (e.g. a CA generated by the script mentioned above), use the
`--allow_unknown_root_cert` flag on the client to allow connections where the
cert chains to a user installed CA.

**note: both the client and server are meant mainly for integration testing:
neither is performant at scale!**

To test the same download using chrome,

```none
chrome \
  --user-data-dir=/tmp/chrome-profile \
  --no-proxy-server \
  --enable-quic \
  --origin-to-force-quic-on=www.example.org:443 \
  --host-resolver-rules='MAP www.example.org:443 127.0.0.1:6121' \
  https://www.example.org
```

Note that the server's certificate must be trusted by a default CA for
Chrome/Chromium to accept it for QUIC. If you are using a self-signed
certificate or a certificate that is signed by a custom CA, you need to use the
`--ignore-certificate-errors-spki-list` command line flag to trust an individual
certificate based on its SPKI. It is not possible to trust a custom CA using this
flag. If you wish to deploy a MITM proxy that intercepts traffic, you need to
block QUIC entirely and intercept TLS instead.

### **Troubleshooting**

If you run into troubles, try running the server or client with --v=1. It will
increase the logging verbosity and the additional logs will often help expose
the underlying problem.
