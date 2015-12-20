<HTML>
<HEAD>
<TITLE> TEST TABLE </TITLE>
</HEAD>
<BODY>
<TABLE BORDER=2>
<%
    int n = 10;
    for ( int i = 0; i < n; i++ ) {
        %>
        <TR>
        <TD>Number</TD>
        <TD><%= i+1 %></TD>
        </TR>
        <%
    }
%>
</TABLE>
</BODY>
</HTML>
