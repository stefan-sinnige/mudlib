<?xml version="1.0"?>
<!--
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
-->
<!--
  This XSL stylesheet transforms a muddoc generated documentation of a
  certain header file to an HTML based reference. The stylesheet is expected
  to be run against libxslt's xsltproc as it uses some specific extensions that
  are XSL 2.0, but not provided as such by libxslt. This include:
    * Using func:function instead of xsl:function
    * Using set:distinct instead of xsl:distinct
 -->
<xsl:stylesheet
    version="1.0"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:exsl="http://exslt.org/common"
    xmlns:func="http://exslt.org/functions"
    xmlns:set="http://exslt.org/sets"
    extension-element-prefixes="exsl func set"
    exclude-result-prefixes="xs">

  <!-- The Header file, as passed-in -->
  <xsl:param name="header-file"/>

  <!-- Generating HTML -->
  <xsl:output method="html" indent="yes" omit-xml-declaration="yes"/>

  <!--
    Return the relative path (as a number of '../') to the base folder for the
    parameter 'path'. For example, 'mud/core/bigint.h' will return '../../'
    -->
  <xsl:template name="relative-path">
    <xsl:param name="path"/>
    <xsl:variable name="remaining" select="substring-after($path, '/')"/>
    <!-- <xsl:value-of select="concat('../', $remaining)"/> -->
    <xsl:choose>
      <!-- <xsl:when test="string-length($remaining) = 0"> -->
      <xsl:when test="contains($remaining, '/')">
        <xsl:text>../</xsl:text>
        <xsl:call-template name="relative-path">
          <xsl:with-param name="path" select="$remaining"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text>../</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!--
    Return the fully-qualitied namespace of the current element. Each namespace
    entry is separated by '::' and may be a class or a namespace element. It
    does not include the element itself.
    -->
  <func:function name="func:namespace">
    <xsl:variable name="ns-elements">
      <xsl:for-each select="ancestor::namespace | ancestor::class">
        <xsl:value-of select="concat(@name,'::')"/>
      </xsl:for-each>
    </xsl:variable>
    <func:result select="$ns-elements"/>
  </func:function>

  <!--
    The documentation of the physical file.
    -->
  <xsl:template match="translation-unit">
    <xsl:variable name="relative-base">
      <xsl:call-template name="relative-path">
        <xsl:with-param name="path" select="$header-file"/>
      </xsl:call-template>
    </xsl:variable>
    <html>
    <head>
      <title><xsl:value-of select="$header-file"/></title>
      <link rel="stylesheet" href="{concat($relative-base,'mudlib.css')}"/>
      <script type="text/javascript" src="{concat($relative-base,'highlight.js')}"/>
    </head>
    <body>
      <header>
        <img src="{concat($relative-base,'mud-small.png')}" width="50" height="50"/>
        MUD Library Reference
      </header>
      <div class="navigation">
      </div>
      <div class="container">
        <div class="side">
          Contents
          <ul>
            <xsl:call-template name="contents">
              <xsl:with-param name="type">typedef</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="contents">
              <xsl:with-param name="type">struct</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="contents">
              <xsl:with-param name="type">class</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="contents">
              <xsl:with-param name="type">function</xsl:with-param>
            </xsl:call-template>
          </ul>
        </div>
        <div class="main">
          <xsl:choose>
            <xsl:when test="//namespace/typedef">
              <h1>Type Definitions</h1>
              <table>
                <xsl:apply-templates select ="//namespace/typedef"/>
              </table>
            </xsl:when>
          </xsl:choose>
          <xsl:choose>
            <xsl:when test="//namespace/struct">
              <h1>Structures</h1>
              <xsl:apply-templates select ="//namespace/struct"/>
            </xsl:when>
          </xsl:choose>
          <xsl:choose>
            <xsl:when test="//namespace/class">
              <h1>Classes</h1>
              <xsl:apply-templates select ="//namespace/class"/>
            </xsl:when>
          </xsl:choose>
          <xsl:choose>
            <xsl:when test="//namespace/function">
              <h1>Functions</h1>
              <table>
                <xsl:apply-templates select ="//namespace/function"/>
              </table>
            </xsl:when>
          </xsl:choose>
        </div>
      </div>
      <footer>
        Public source-code repository: <a href="https://github.com/stefan-sinnige/mudlib">github</a>
      </footer>
    </body>
    </html>
  </xsl:template>

  <!--
    Add a contents table
    -->
  <xsl:template name="contents">
    <xsl:param name="type"/>
    <xsl:variable name="title">
      <xsl:choose>
        <xsl:when test="$type = 'class'">Classes</xsl:when>
        <xsl:when test="$type = 'function'">Functions</xsl:when>
        <xsl:when test="$type = 'struct'">Structures</xsl:when>
        <xsl:when test="$type = 'typedef'">Type Definitions</xsl:when>
      </xsl:choose>
    </xsl:variable>
    <xsl:value-of select="$title"/>
    <ul>
      <xsl:apply-templates select="//namespace/*[local-name() = $type]" mode="contents">
        <xsl:sort select="name"/>
      </xsl:apply-templates>
    </ul>
  </xsl:template>

  <xsl:template match="typedef | function" mode="contents">
    <xsl:variable name="name">
      <xsl:value-of select="name"/>
    </xsl:variable>
    <li><a href="#id-{$name}"><xsl:value-of select="$name"/></a></li>
  </xsl:template>

  <xsl:template match="class | struct" mode="contents">
    <xsl:variable name="name">
      <xsl:value-of select="name"/>
    </xsl:variable>
    <li><a href="#id-{$name}"><xsl:value-of select="$name"/></a></li>
    <ul>
      <xsl:apply-templates select="*[local-name() = local-name(current())]" mode="contents"/>
    </ul>
  </xsl:template>

  <!--
    Apply the documentation of a type definition
    -->
  <xsl:template match="typedef">
    <xsl:variable name="name">
      <xsl:value-of select="name"/>
    </xsl:variable>
    <tr>
      <td colspan="2">
        <div id="id-{$name}" class="codeblock"><xsl:value-of select="declaration"/></div>
      </td>
    </tr>
    <tr>
      <td colspan="2">
        <xsl:apply-templates select="comment/brief" mode="heading"/>
        <xsl:apply-templates select="comment/detail" mode="heading"/>
      </td>
    </tr> 
    <tr>
      <td colspan="2" class="separator"></td>
    </tr>
  </xsl:template>

  <!--
    Apply the documentation of a struct definition
    -->
  <xsl:template match="struct">
    <xsl:variable name="name">
      <xsl:value-of select="name"/>
    </xsl:variable>
    <h2 id="id-{$name}">Structure <xsl:value-of select="qualified-name"/></h2>
    Defined in header <span class="monospace">&lt;<xsl:value-of select="$header-file"/>&gt;</span>
  </xsl:template>

  <!--
    Apply the documentation of a class
    -->
  <xsl:template match="class">
    <xsl:variable name="name">
      <xsl:value-of select="name"/>
    </xsl:variable>
    <h2 id="id-{$name}"><xsl:value-of select="qualified-name"/></h2>
    Defined in header <span class="monospace">&lt;<xsl:value-of select="$header-file"/>&gt;</span>
    <xsl:apply-templates select="comment/template-parameters" mode="heading"/>
    <xsl:apply-templates select="comment/brief" mode="heading"/>
    <xsl:apply-templates select="comment/detail" mode="heading"/>
    <xsl:choose>
      <xsl:when test="typedef">
        <h3>Member Types</h3>
        <table class="methods">
          <xsl:apply-templates select="typedef">
            <xsl:sort select="name"/>
          </xsl:apply-templates>
        </table>
      </xsl:when>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test="method[contains(@kind, 'structor')]">
        <h3>Constructors and Destructor</h3>
        <table class="methods">
          <xsl:apply-templates select="method[contains(@kind, 'structor')]">
            <xsl:sort select="name"/>
          </xsl:apply-templates>
        </table>
      </xsl:when>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test="method[contains(@kind, 'assignment')]">
        <h3>Assignment Operators</h3>
        <table class="methods">
          <xsl:apply-templates select="method[contains(@kind, 'assignment')]">
            <xsl:sort select="name"/>
          </xsl:apply-templates>
        </table>
      </xsl:when>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test="method[contains(@kind, 'operator')]">
        <h3>Operators</h3>
        <table class="methods">
          <xsl:apply-templates select="method[contains(@kind, 'operator')]">
            <xsl:sort select="name"/>
          </xsl:apply-templates>
        </table>
      </xsl:when>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test="method[contains(@kind, 'method')]">
        <h3>Member Functions</h3>
        <table class="methods">
          <xsl:apply-templates select="method[contains(@kind, 'method')]">
            <xsl:sort select="name"/>
          </xsl:apply-templates>
        </table>
      </xsl:when>
    </xsl:choose>
    <!-- Inner classes -->
    <xsl:apply-templates select ="class">
      <xsl:sort select="name"/>
    </xsl:apply-templates>
  </xsl:template>

  <!--
    A full detailed description of a function or method.
    -->
  <xsl:template match="function | method">
    <xsl:variable name="name">
      <xsl:value-of select="name"/>
    </xsl:variable>
    <tr>
      <td colspan="2">
        <div id="id-{$name}" class="codeblock">
            <xsl:value-of select="declaration"/>
        </div>
      </td>
    </tr> 
    <xsl:apply-templates select="comment/parameters" mode="heading"/>
    <xsl:apply-templates select="comment/return" mode="heading"/>
    <xsl:apply-templates select="comment/exceptions" mode="heading"/>
    <tr>
      <td colspan="2">
        <xsl:apply-templates select="comment/brief" mode="heading"/>
        <xsl:apply-templates select="comment/detail" mode="heading"/>
      </td>
    </tr> 
    <xsl:apply-templates select="comment/note" mode="misc"/>
    <xsl:apply-templates select="comment/remark" mode="misc"/>
    <xsl:apply-templates select="comment/author" mode="misc"/>
    <tr>
      <td colspan="2" class="separator"></td>
    </tr>
  </xsl:template>

  <!-- Brief comment -->
  <xsl:template match="brief | detail" mode="heading">
    <xsl:variable name="name">
      <xsl:value-of select="name()"/>
    </xsl:variable>
    <div class="{$name}">
      <xsl:apply-templates select="."/>
    </div>
  </xsl:template>

  <!-- Template Parameters with heading -->
  <xsl:template match="template-parameters" mode="heading">
    <h4>Template Parameters</h4>
    <table>
      <xsl:apply-templates select="parameter"/>
    </table>
  </xsl:template>

  <!-- Parameters with heading -->
  <xsl:template match="parameters" mode="heading">
    <tr>
      <td colspan="2">
        <h4>Parameters</h4>
      </td>
    </tr> 
    <xsl:apply-templates select="parameter"/>
  </xsl:template>

  <!-- A function or template parameter -->
  <xsl:template match="parameter">
    <HERE/>
    <tr>
      <td class="code parameter"><xsl:value-of select="name"/></td>
      <td><xsl:apply-templates select="detail"/></td>
    </tr> 
  </xsl:template>

  <!-- Return value with heading -->
  <xsl:template match="return" mode="heading">
    <tr>
      <td colspan="2">
        <h4>Return Value</h4>
      </td>
    </tr> 
    <tr>
      <td colspan="2" class="return"><xsl:apply-templates select="."/></td>
    </tr> 
  </xsl:template>

  <!-- Exceptions with heading -->
  <xsl:template match="exceptions" mode="heading">
    <tr>
      <td colspan="2">
        <h4>Exceptions</h4>
      </td>
    </tr> 
    <td><xsl:apply-templates select="exception"/></td>
  </xsl:template>
  <xsl:template match="exception">
    <tr>
      <td class="code exception"><xsl:value-of select="type"/></td>
      <td><xsl:apply-templates select="text()"/></td>
    </tr> 
  </xsl:template>

  <!-- Miscellaneous -->
  <xsl:template match="note | author | remark" mode="misc">
    <xsl:variable name="name">
      <xsl:value-of select="name()"/>
    </xsl:variable>
    <xsl:variable name="title">
      <xsl:choose>
        <xsl:when test="$name = 'note'">Notes</xsl:when>
        <xsl:when test="$name = 'author'">Authors</xsl:when>
        <xsl:when test="$name = 'remark'">Remarks</xsl:when>
      </xsl:choose>
    </xsl:variable>
    <tr>
      <td colspan="2">
        <h5><xsl:value-of select="$title"/></h5>
      </td>
    </tr> 
    <tr>
      <td colspan="2" class="misc"><xsl:apply-templates select="."/></td>
    </tr> 
  </xsl:template>

  <!-- Example code -->
  <xsl:template match="code">
    <div class="example">
    <xsl:value-of select="text()"/>
    </div>
  </xsl:template>

  <!-- Monospaced text -->
  <xsl:template match="monospaced">
    <tt><xsl:value-of select="."/></tt>
  </xsl:template>

  <!-- Bold text -->
  <xsl:template match="bold">
    <b><xsl:value-of select="."/></b>
  </xsl:template>

  <!-- Emphasized text -->
  <xsl:template match="emphasized">
    <em><xsl:value-of select="."/></em>
  </xsl:template>

  <xsl:template match="ordered">
    <ol><xsl:apply-templates select="item"/></ol>
  </xsl:template>

  <xsl:template match="unordered">
    <ul><xsl:apply-templates select="item"/></ul>
  </xsl:template>

  <xsl:template match="item">
    <li><xsl:apply-templates/></li>
  </xsl:template>

</xsl:stylesheet>
