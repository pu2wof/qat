all: qat-qsharp

qat-qsharp: QSharpVersion/qir/Example.ll
	${QAT_BINARY} --apply -O3 --verify-module --validate --always-inline --unroll-loops --output test.bc --profile default QSharpVersion/qir/Example.ll #> combined.ll
	

QSharpVersion/qir/Example.ll:
	cd QSharpVersion && make qir/Example.ll

clean:
	cd QSharpVersion && make clean
	rm -f combined.ll optimized.ll
	rm -f validation.json