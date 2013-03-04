(defun make-param-list (params)
  (if params
      (let ((head-decl
	     (concatenate 'string (cadar params) " _" (caar params))))
	(if (cdr params)
	    (concatenate 'string head-decl ", " (make-param-list (cdr params)))
	    head-decl))))

(defun make-decls (members)
  (if members
      (concatenate 'string (cadar members) " " (caar members) ";"
		   (make-decls (cdr members)))))

(defun make-classdecl (name members)
  (concatenate 'string "struct " name " : AST {"
	       (make-decls members)
	       name "();"
	       name "(" (make-param-list members) ");"
	       "virtual std::vector <std::string> contents () const override; };"))

(defun make-init-list (members)
  (if members
      (let ((head-def
	     (concatenate 'string (caar members) "(_" (caar members) ") ")))
	(if (cdr members)
	    (concatenate 'string head-def ", " (make-init-list (cdr members)))
	    head-def))))

(defun make-classdef (name members)
  (concatenate 'string name "::" name " (" (make-param-list members) ")"
	       ": " (make-init-list members) "{type = AST_type::" name ";}"
	       #(#\Newline) name "::" name " () {type = AST_type::"
	       name ";}"))

(defun make-linegets (members)
  (let ((head-decl
	 (concatenate 'string "valid (" (caar members) ") ? lines (" (caar members) ") : vector <string> {} ")))
    (if (cdr members)
	(concatenate 'string head-decl ", " (make-linegets (cdr members)))
	head-decl)))

(defun make-contents (name members)
  (concatenate 'string "vector <string> " name "::contents () const {"
	       "return collect (" (make-linegets members) "); }"))

(defparameter ast-types `(("program"        (("decl_list" "Node <declList>")))

			  ("declList"       (("decls"     "std::vector <Node <decl>>")))

			  ("decl"           (("sub_decl"  "AST_node")))

			  ("varDecl"        (("type_spec" "Node <typeSpecifier>")
			  		     ("name"      "Node <terminal>")
			  		     ("size"      "Node <terminal>")))

			  ("typeSpecifier"  (("kwd_node"  "Node <terminal>")))

			  ("funDecl"        (("type_spec" "Node <typeSpecifier>")
			  		     ("name"      "Node <terminal>")
			  		     ("decl_list" "Node <formalDeclList>")
			  		     ("body"      "Node <funBody>")))

			  ("formalDeclList" (("decls"     "std::vector <Node <formalDecl>>")))

			  ("formalDecl"     (("type_spec" "Node <typeSpecifier>")
					     ("name"      "Node <terminal>")
					     ("is_array"  "bool")))

			  ("funBody"        (("decl_list" "Node <localDeclList>")
					     ("stmt_list" "Node <statementList>")))

			  ("localDeclList"  (("decls"     "std::vector <Node <varDecl>>")))

			  ("statementList"  (("stmts"     "std::vector <Node <statement>>")))

			  ("statement"      (("sub_stmt"  "AST_node")))

			  ("compoundStmt"   (("stmt_list" "Node <statementList>")))

			  ("assignStmt"     (("lvalue"    "Node <var>")
					     ("rvalue"    "Node <expression>")))

			  ("condStmt"       (("cond_expr" "Node <expression>")
					     ("then_stmt" "Node <statement>")
					     ("else_stmt" "Node <statement>")))

			  ("loopStmt"       (("cond_expr" "Node <expression>")
					     ("then_stmt" "Node <statement>")))

			  ("returnStmt"     (("rtrn_expr" "Node <expression>")))

			  ("var"            (("name"      "Node <terminal>")
					     ("size"      "Node <addExpr>")))

			  ("expression"     (("lhs"       "Node <expression>")
					     ("op"        "Node <relop>")
					     ("rhs"       "Node <addExpr>")))

			  ("relop"          (("sym"       "Node <terminal>")))

			  ("addExpr"        (("lhs"       "Node <addExpr>")
					     ("op"        "Node <addop>")
					     ("rhs"       "Node <term>")))

			  ("addop"          (("sym"       "Node <terminal>")))

			  ("term"           (("lhs"       "Node <term>")
					     ("op"        "Node <mulop>")
					     ("rhs"       "Node <factor>")))

			  ("mulop"          (("sym"       "Node <terminal>")))

			  ("factor"         (("rvalue"    "AST_node")))

			  ("funcCallExpr"   (("name"      "Node <terminal>")
					     ("arg_list"  "Node <argList>")))

			  ("argList"        (("args"      "std::vector <Node <expression>>")))))

(defun make-declfile (pathname)
  (with-open-file (declfile pathname
			    :if-exists :supersede
			    :if-does-not-exist :create
			    :direction :output)
    (loop for typespec in ast-types do
	 (write-string (concatenate 'string
				    (make-classdecl (car typespec) (cadr typespec))
				    #(#\Newline))
		       declfile))))

(defun make-deffile (pathname)
  (with-open-file (deffile pathname
		      :if-exists :supersede
		      :if-does-not-exist :create
		      :direction :output)
    (loop for typespec in ast-types do
	 (write-string (concatenate 'string
				    (make-classdef (car typespec) (cadr typespec))
				    #(#\Newline)
				    (make-contents (car typespec) (cadr typespec))
				    #(#\Newline))
		       deffile))))
