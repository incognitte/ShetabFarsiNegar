<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent ="yes" encoding ="utf-8" />

  <xsl:template match="*">
    <xsl:copy>
      <xsl:apply-templates select ="@*"/>
      <xsl:apply-templates select ="*"/>
      <xsl:value-of select ="text()"/>
    </xsl:copy>
  </xsl:template>
  
  <xsl:template match="Glyphs">
    <xsl:copy>
      <xsl:copy-of  select="*[name()!='Glyph']"/>
      <xsl:apply-templates select="Glyph/@Code">
        <xsl:sort data-type ="text"/>
      </xsl:apply-templates>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="Glyph/@Code">
    <xsl:copy-of select =".."/>
  </xsl:template>

  <xsl:template match="@*">
    <xsl:copy>
      <xsl:apply-templates select="@*"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>