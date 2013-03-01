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
	       name "(" (make-param-list members) ");"
	       "virtual std::vector <std::string> contents () override; };"))

(defun make-init-list (members)
  (if members
      (concatenate 'string ", " (caar members) "(_" (caar members) ") ")))

(defun make-classdef (name members)
  (concatenate 'string name "::" name " (" (make-param-list members) ")"
	       ": type (AST_type::" name ")" (make-init-list members) "{}"))

(defun make-linegets (members)
  (let ((head-decl
	 (concatenate 'string (caar members) "? lines (" (caar members) ") : {} ")))
    (if (cdr members)
	(concatenate 'string head-decl ", " (make-linegets (cdr members)))
	head-decl)))

(defun make-contents (name members)
  (concatenate 'string "virtual vector <string> " name "::contents () {"
	       "return collect (" (make-linegets members) "); }"))

(defparameter node "AST_node")
(defparameter nlist "std::vector <AST_node>")

(defparameter ast-types `(("program"        (("decl_list" ,node)))

			  ("declList"       (("decls"     ,nlist)))

			  ("decl"           (("sub_decl"  ,node)))

			  ("varDecl"        (("type_spec" ,node)
					     ("name"      ,node)
					     ("size"      ,node)))

			  ("typeSpecifier"  (("kwd_node"  ,node)))

			  ("funDecl"        (("type_spec" ,node)
					     ("name"      ,node)
					     ("decl_list" ,node)
					     ("body"      ,node)))

			  ("formalDeclList" (("decls"     ,nlist)))

			  ("formalDecl"     (("type_spec" ,node)
					     ("name"      ,node)
					     ("is_array"  "bool")))

			  ("funBody"        (("decl_list" ,node)
					     ("stmt_list" ,node)))

			  ("localDeclList"  (("decls"     ,nlist)))

			  ("statementList"  (("stmts"     ,nlist)))

			  ("statement"      (("sub_stmt"  ,node)))

			  ("compoundStmt"   (("stmt_list" ,node)))

			  ("assignStmt"     (("lvalue"    ,node)
					     ("rvalue"    ,node)))

			  ("condStmt"       (("cond_expr" ,node)
					     ("then_stmt" ,node)
					     ("else_stmt" ,node)))

			  ("loopStmt"       (("cond_expr" ,node)
					     ("then_stmt" ,node)))

			  ("returnStmt"     (("rtrn_expr" ,node)))

			  ("var"            (("name"      ,node)
					     ("size"      ,node)))

			  ("expression"     (("lhs"       ,node)
					     ("op"        ,node)
					     ("rhs"       ,node)))

			  ("relop"          (("sym"       ,node)))

			  ("addExpr"        (("lhs"       ,node)
					     ("op"        ,node)
					     ("rhs"       ,node)))

			  ("addop"          (("sym"       ,node)))

			  ("term"           (("lhs"       ,node)
					     ("op"        ,node)
					     ("rhs"       ,node)))

			  ("mulop"          (("sym"       ,node)))

			  ("factor"         (("rvalue"    ,node)))

			  ("funcCallExpr"   (("name"      ,node)
					     ("arg_list"  ,node)))

			  ("argList"        (("args"      ,nlist)))))

;; (with-open-file (declfile "auto.astdecls.hh"
;; 			  :if-exists :supersede
;; 			  :if-does-not-exist :create
;; 			  :direction :output)
;;   (loop for typespec in ast-types do
;;        (write-string (concatenate 'string
;; 				  (make-classdecl (car typespec) (cadr typespec))
;; 				  #(#\Newline))
;; 		     declfile)))

;; (with-open-file (deffile "auto.astdefs.hh"
;; 			  :if-exists :supersede
;; 			  :if-does-not-exist :create
;; 			  :direction :output)
;;   (loop for typespec in ast-types do
;;        (write-string (concatenate 'string
;; 				  (make-classdef (car typespec) (cadr typespec))
;; 				  #(#\Newline)
;; 				  (make-contents (car typespec) (cadr typespec))
;; 				  #(#\Newline))
;; 		     deffile)))