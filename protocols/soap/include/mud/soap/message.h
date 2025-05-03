#ifndef _MUDLIB_SOAP_MESSAGE_H_
#define _MUDLIB_SOAP_MESSAGE_H_

#include <mud/soap/ns.h>
#include <mud/xml/document.h>
#include <iostream>
#include <string>

BEGIN_MUDLIB_SOAP_NS

/**
 * @brief The definition of a SOAP message.
 *
 * @details
 * SOAP (Simple Object Access Protocol) is a messaging protocol to exchange
 * structured information. The message itself is formatted as XML Information
 * Set and is designed to be used over a a number of application layer
 * protocols. 
 *
 * The usage of SOAP at an application layer is called a SOAP @em binding. HTTP
 * is the the most prevalent form of such a binding, in which case it is often
 * refered to as a Web Service. The binding often imposes certain assumptions
 * and restrictions on the underlying messaging protocol.
 *
 * A SOAP message is typically sent as part of a one-way communication framework
 * between a sender and a receiver. The message is constructed as a defined
 * @em envelope that contains a header and a body. The header part is generally
 * used to convey meta-data which is not considered to be part of the payload.
 * This may, for example, contain information like a timestamp when the SOAP
 * message is sent. The body will contain the actual payload of the message.
 * The structure definition of the header and body are @em not defined by the
 * SOAP specification but form part of the protocol where the SOAP message is
 * used as a payload mechanism. For web-services, the body is often described
 * in the form of a Web Service Definition Language (WSDL) but this is outside
 * the scope of this SOAP protocol package. The SOAP message is likely to be
 * used by the sender in its request and the receiver in its reply.
 *
 * For example, a travel company may send a reservation request like
 * @code
 * <?xml version='1.0' ?>
 * <env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" >
 *   <env:Header>
 *     <t:transaction
 *          xmlns:t="http://thirdparty.example.org/transaction"
 *          env:encodingStyle="http://example.com/encoding"
 *          env:mustUnderstand="true" >5</t:transaction>
 *   </env:Header>  
 *   <env:Body>
 *     <m:chargeReservation
 *          env:encodingStyle="http://www.w3.org/2003/05/soap-encoding"
 *          xmlns:m="http://travel.example.org/">
 *       <m:reservation xmlns:m="http://travel.example.org/reservation">
 *         <m:code>FT35ZBQ</m:code>
 *       </m:reservation>
 *       <o:creditCard xmlns:o="http://mycompany.example.com/financial">
 *         <o:name>Åke Jógvan Øyvind</o:name>
 *         <o:number>123456789099999</o:number>
 *         <o:expiration>2005-02</o:expiration>
 *       </o:creditCard>
 *     </m:chargeReservation>
 *   </env:Body>
 * </env:Envelope>
 * @endcode
 * which may be responded with
 * @code
 * <?xml version='1.0' ?>
 * <env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" >
 *   <env:Header>
 *     <t:transaction
 *          xmlns:t="http://thirdparty.example.org/transaction"
 *          env:encodingStyle="http://example.com/encoding"
 *          env:mustUnderstand="true">5</t:transaction>
 *   </env:Header>  
 *   <env:Body>
 *     <m:chargeReservationResponse 
 *          xmlns:m="http://travel.example.org/"
 *          env:encodingStyle="http://www.w3.org/2003/05/soap-encoding">
 *       <m:code>FT35ZBQ</m:code>
 *     </m:chargeReservationResponse>
 *   </env:Body>
 * </env:Envelope>
 * @endcode
 *
 * Any fault that is encountered at the SOAP level, or at the payload processing
 * facility may use the SOAP Fault message. This message is a SOAP message whose
 * body caries a well-defined Fault payload to procide details about the
 * fault as a code, a reason or any structured details sections (again allowing
 * customised XML structures to relay the fault details back to the peer).
 *
 * An example of a Fault message:
 * @code
 * <?xml version='1.0' ?>
 * <env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"
 *       xmlns:rpc='http://www.w3.org/2003/05/soap-rpc'>
 *   <env:Body>
 *     <env:Fault>
 *       <env:Code>
 *          <env:Value>env:Sender</env:Value>
 *          <env:Subcode>
 *            <env:Value>rpc:BadArguments</env:Value>
 *          </env:Subcode>
 *       </env:Code>
 *       <env:Reason>
 *         <env:Text xml:lang="en-US">Processing error</env:Text>
 *         <env:Text xml:lang="cs">Chyba zpracování</env:Text>
 *       </env:Reason>
 *       <env:Detail>
 *         <e:myFaultDetails xmlns:e="http://travel.example.org/faults">
 *           <e:message>Name does not match card number</e:message>
 *           <e:errorcode>999</e:errorcode>
 *         </e:myFaultDetails>
 *       </env:Detail>
 *     </env:Fault>
 *   </env:Body>
 * </env:Envelope>
 * @endcode
 *
 * The SOAP specification implemented is version 1.2 and is governed by W3C.
 */
class MUDLIB_SOAP_API message
{
public:
    /**
     * @brief Create an empty SOAP message.
     *
     * @details
     * An empty SOAP message consists of an Envelope and an empty Header and
     * Body.
     */
    message();

    /**
     * @brief Delete a SOAP message.
     */
    virtual ~message() = default;

    /**
     * @brief Return the SOAP message as an XML Information Set.
     */
    mud::xml::document::ptr xml() const;

private:
    /** The XML Information Set document. */
    mud::xml::document::ptr _xml; 
};

END_MUDLIB_SOAP_NS

/** Read a SOAP message from an input stream. */
std::istream&
operator>>(std::istream&, mud::soap::message&);

/** Write an SOAP message to an output stream. */
std::ostream&
operator<<(std::ostream&, const mud::soap::message&);

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_SOAP_MESSAGE_H_ */
