package net.meanscript.java;
import net.meanscript.core.*;
public interface MJCallbackAction
{
	void action(MeanMachine mm, MArgs args) throws MException;
}